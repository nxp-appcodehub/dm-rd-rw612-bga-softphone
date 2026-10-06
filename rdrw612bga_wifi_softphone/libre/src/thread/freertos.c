/**
 * @file posix.c  Pthread C11 thread implementation
 *
 * Copyright (C) 2022 Sebastian Reimers
 */
#include <re_types.h>
#include <re_mem.h>
#include <re_thread.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include <string.h>

#define TSS_DESTRUCTOR_MAX 64
#define TSS_MAX_KEYS 32

/* TSS (Thread Specific Storage) implementation */
static struct tss_dtor_entry {
	tss_t key;
	tss_dtor_t dtor;
	bool used;
} tss_dtor_tbl[TSS_DESTRUCTOR_MAX];

struct tss_storage {
	void *data[TSS_MAX_KEYS];
	bool initialized;
};

static SemaphoreHandle_t tss_mutex = NULL;
static bool tss_keys[TSS_MAX_KEYS] = {false};

struct thread {
	thrd_start_t func;
	void *arg;
	const char *name;
};

static void tss_dtor_destruct(void);

static void handler(void *p)
{
	struct thread th = *(struct thread *)p;

	mem_deref(p);

	/* Set task name if provided */
	// TODO: Enable Thread naming
	if (th.name) {
		//vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)th.name);
	}

	th.func(th.arg);

	tss_dtor_destruct();

	vTaskDelete(NULL);
}


int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	struct thread *th;
	BaseType_t ret;

	if (!thr || !func)
		return thrd_error;

	th = mem_alloc(sizeof(struct thread), NULL);
	if (!th)
		return thrd_nomem;

	th->func = func;
	th->arg	 = arg;
	th->name = NULL;

	ret = xTaskCreate(handler, "thread",
			  configMINIMAL_STACK_SIZE * 4, th,
			  tskIDLE_PRIORITY + 1, thr);

	if (ret != pdPASS) {
		mem_deref(th);
		return (ret == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) ?
			thrd_nomem : thrd_error;
	}

	return thrd_success;
}


int thrd_equal(thrd_t lhs, thrd_t rhs)
{
	return (lhs == rhs) ? 1 : 0;
}


thrd_t thrd_current(void)
{
	return xTaskGetCurrentTaskHandle();
}


int thrd_detach(thrd_t thr)
{
	/* FreeRTOS doesn't have detach concept, just return success */
	(void)thr;
	return thrd_success;
}


int thrd_join(thrd_t thr, int *res)
{
	/* FreeRTOS doesn't have native join, we'll implement a simple wait */
	while (eTaskGetState(thr) != eDeleted) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	if (res)
		*res = 0; /* FreeRTOS doesn't return exit codes */

	return thrd_success;
}


void call_once(once_flag *flag, void (*func)(void))
{
	if (!flag || !func)
		return;

	taskENTER_CRITICAL();
	if (!flag->done) {
		func();
		flag->done = true;
	}
	taskEXIT_CRITICAL();
}


void thrd_exit(int res)
{
	(void)res;
	vTaskDelete(NULL);
}


int cnd_init(cnd_t *cnd)
{
	if (!cnd)
		return thrd_error;

	cnd->sem = xSemaphoreCreateCounting(UINT32_MAX, 0);
	cnd->mutex = xSemaphoreCreateMutex();
	cnd->waiters = 0;

	if (!cnd->sem || !cnd->mutex) {
		if (cnd->sem) {
			vSemaphoreDelete(cnd->sem);
			cnd->sem = NULL;
		}
		if (cnd->mutex) {
			vSemaphoreDelete(cnd->mutex);
			cnd->mutex = NULL;
		}
		return thrd_error;
	}

	return thrd_success;
}


int cnd_signal(cnd_t *cnd)
{
	if (!cnd || !cnd->mutex || !cnd->sem)
		return thrd_error;

	/* Take the internal mutex to protect waiters count */
	if (xSemaphoreTake(cnd->mutex, portMAX_DELAY) == pdTRUE) {
		if (cnd->waiters > 0) {
			/* Signal one waiting thread */
			xSemaphoreGive(cnd->sem);
		}
		xSemaphoreGive(cnd->mutex);
	}

	return thrd_success;
}


int cnd_broadcast(cnd_t *cnd)
{
	uint32_t waiters_to_wake;

	if (!cnd || !cnd->mutex || !cnd->sem)
		return thrd_error;

	/* Take the internal mutex to protect waiters count */
	if (xSemaphoreTake(cnd->mutex, portMAX_DELAY) == pdTRUE) {
		waiters_to_wake = cnd->waiters;

		/* Signal all waiting threads */
		for (uint32_t i = 0; i < waiters_to_wake; i++) {
			xSemaphoreGive(cnd->sem);
		}

		xSemaphoreGive(cnd->mutex);
	}

	return thrd_success;
}


int cnd_wait(cnd_t *cnd, mtx_t *mtx)
{
	if (!cnd || !mtx || !cnd->mutex || !cnd->sem || !*mtx)
		return thrd_error;

	/* Increment waiters count */
	if (xSemaphoreTake(cnd->mutex, portMAX_DELAY) == pdTRUE) {
		cnd->waiters++;
		xSemaphoreGive(cnd->mutex);
	} else {
		return thrd_error;
	}

	/* Release the user mutex */
	xSemaphoreGive(*mtx);

	/* Wait for signal */
	xSemaphoreTake(cnd->sem, portMAX_DELAY);

	/* Reacquire the user mutex */
	xSemaphoreTake(*mtx, portMAX_DELAY);

	/* Decrement waiters count */
	if (xSemaphoreTake(cnd->mutex, portMAX_DELAY) == pdTRUE) {
		cnd->waiters--;
		xSemaphoreGive(cnd->mutex);
	}

	return thrd_success;
}


int cnd_timedwait(cnd_t *cnd, mtx_t *mtx, const struct timespec *abstime)
{
	TickType_t timeout_ticks;
	BaseType_t result;

	if (!cnd || !mtx || !abstime || !cnd->mutex || !cnd->sem || !*mtx)
		return thrd_error;

	/* Calculate timeout in ticks from current time */
	/* Note: This is a simplified conversion - you may need better time handling */
	timeout_ticks = pdMS_TO_TICKS(abstime->tv_sec * 1000 +
				      abstime->tv_nsec / 1000000);

	/* Increment waiters count */
	if (xSemaphoreTake(cnd->mutex, portMAX_DELAY) == pdTRUE) {
		cnd->waiters++;
		xSemaphoreGive(cnd->mutex);
	} else {
		return thrd_error;
	}

	/* Release the user mutex */
	xSemaphoreGive(*mtx);

	/* Wait for signal with timeout */
	result = xSemaphoreTake(cnd->sem, timeout_ticks);

	/* Reacquire the user mutex */
	xSemaphoreTake(*mtx, portMAX_DELAY);

	/* Decrement waiters count */
	if (xSemaphoreTake(cnd->mutex, portMAX_DELAY) == pdTRUE) {
		cnd->waiters--;
		xSemaphoreGive(cnd->mutex);
	}

	return (result == pdTRUE) ? thrd_success : thrd_timedout;
}


void cnd_destroy(cnd_t *cnd)
{
	if (!cnd)
		return;

	if (cnd->sem) {
		vSemaphoreDelete(cnd->sem);
		cnd->sem = NULL;
	}

	if (cnd->mutex) {
		vSemaphoreDelete(cnd->mutex);
		cnd->mutex = NULL;
	}

	cnd->waiters = 0;
}


int mtx_init(mtx_t *mtx, int type)
{
	if (!mtx)
		return thrd_error;

	if (type & mtx_recursive) {
		*mtx = xSemaphoreCreateRecursiveMutex();
	} else {
		*mtx = xSemaphoreCreateMutex();
	}

	return (*mtx != NULL) ? thrd_success : thrd_error;
}


int mtx_lock(mtx_t *mtx)
{
	if (!mtx || !*mtx)
		return thrd_error;

	/* Check if it's a recursive mutex by trying to determine type */
	/* This is a simplification - FreeRTOS doesn't provide easy type checking */
	BaseType_t result = xSemaphoreTake(*mtx, portMAX_DELAY);

	return (result == pdTRUE) ? thrd_success : thrd_error;
}


int mtx_trylock(mtx_t *mtx)
{
	if (!mtx || !*mtx)
		return thrd_error;

	BaseType_t result = xSemaphoreTake(*mtx, 0);

	if (result == pdTRUE)
		return thrd_success;
	else
		return thrd_busy;
}


int mtx_unlock(mtx_t *mtx)
{
	if (!mtx || !*mtx)
		return thrd_error;

	BaseType_t result = xSemaphoreGive(*mtx);

	return (result == pdTRUE) ? thrd_success : thrd_error;
}


void mtx_destroy(mtx_t *mtx)
{
	if (!mtx || !*mtx)
		return;

	vSemaphoreDelete(*mtx);
	*mtx = NULL;
}

/* Initialize TSS system */
static void tss_init(void)
{
	if (!tss_mutex) {
		tss_mutex = xSemaphoreCreateMutex();
	}
}

static int dtor_register(tss_t key, tss_dtor_t dtor)
{
	int i;

	if (!tss_mutex)
		tss_init();

	xSemaphoreTake(tss_mutex, portMAX_DELAY);

	for (i = 0; i < TSS_DESTRUCTOR_MAX; i++) {
		if (!tss_dtor_tbl[i].used)
			break;
	}

	if (i >= TSS_DESTRUCTOR_MAX) {
		xSemaphoreGive(tss_mutex);
		return ENOMEM;
	}

	tss_dtor_tbl[i].key  = key;
	tss_dtor_tbl[i].dtor = dtor;
	tss_dtor_tbl[i].used = true;

	xSemaphoreGive(tss_mutex);
	return 0;
}

static void tss_dtor_destruct(void)
{
	struct tss_storage *storage;
	void *val;

	storage = (struct tss_storage *)pvTaskGetThreadLocalStoragePointer(NULL, 0);
	if (!storage)
		return;

	for (int i = 0; i < TSS_DESTRUCTOR_MAX; i++) {
		if (!tss_dtor_tbl[i].used || !tss_dtor_tbl[i].dtor)
			continue;

		if (tss_dtor_tbl[i].key < TSS_MAX_KEYS) {
			val = storage->data[tss_dtor_tbl[i].key];
			if (val) {
				tss_dtor_tbl[i].dtor(val);
			}
		}
	}

	vPortFree(storage);
}

int tss_create(tss_t *key, tss_dtor_t destructor)
{
	int i;

	if (!key)
		return thrd_error;

	if (!tss_mutex)
		tss_init();

	xSemaphoreTake(tss_mutex, portMAX_DELAY);

	for (i = 0; i < TSS_MAX_KEYS; i++) {
		if (!tss_keys[i]) {
			tss_keys[i] = true;
			*key = i;
			break;
		}
	}

	xSemaphoreGive(tss_mutex);

	if (i >= TSS_MAX_KEYS)
		return thrd_error;

	if (destructor) {
		int err = dtor_register(*key, destructor);
		if (err) {
			tss_keys[*key] = false;
			return thrd_error;
		}
	}

	return thrd_success;
}


void *tss_get(tss_t key)
{
	struct tss_storage *storage;

	if (key >= TSS_MAX_KEYS)
		return NULL;

	storage = (struct tss_storage *)pvTaskGetThreadLocalStoragePointer(NULL, 0);
	if (!storage)
		return NULL;

	return storage->data[key];
}


int tss_set(tss_t key, void *val)
{
	struct tss_storage *storage;

	if (key >= TSS_MAX_KEYS)
		return thrd_error;

	storage = (struct tss_storage *)pvTaskGetThreadLocalStoragePointer(NULL, 0);
	if (!storage) {
		storage = pvPortMalloc(sizeof(struct tss_storage));
		if (!storage)
			return thrd_error;

		memset(storage, 0, sizeof(struct tss_storage));
		storage->initialized = true;
		vTaskSetThreadLocalStoragePointer(NULL, 0, storage);
	}

	storage->data[key] = val;
	return thrd_success;
}


void tss_delete(tss_t key)
{
	if (key >= TSS_MAX_KEYS)
		return;

	if (tss_mutex) {
		xSemaphoreTake(tss_mutex, portMAX_DELAY);
		tss_keys[key] = false;

		/* Remove from destructor table */
		for (int i = 0; i < TSS_DESTRUCTOR_MAX; i++) {
			if (tss_dtor_tbl[i].used && tss_dtor_tbl[i].key == key) {
				tss_dtor_tbl[i].used = false;
				break;
			}
		}

		xSemaphoreGive(tss_mutex);
	}
}
