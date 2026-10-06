/*
 * Copyright 2020-2023, 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "app_streamer.h"
#include "app_definitions.h"
#include "streamer_pcm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* FreeRTOS task stack and priority */
#define STREAMER_TX_TASK_STACK (512U)
#define STREAMER_RX_TASK_STACK (8 * 1024U)
#define STREAMER_TX_TASK_PRIO  (configMAX_PRIORITIES - 2)
#define STREAMER_RX_TASK_PRIO  (configMAX_PRIORITIES - 2)

#define STREAMER_NOTIFY_STOP (1UL << 0)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Baresip callbacks */
static struct
{
    baresip_ausrc_callback_h ausrc_cb;
    void *ausrc_arg;
    baresip_auplay_callback_h auplay_cb;
    void *auplay_arg;
} g_baresip_callbacks = {0};

/* TX task state */
static TaskHandle_t s_txTaskHandle       = NULL; /* handle of the running TX task */
static TaskHandle_t s_txStopCallerHandle = NULL; /* handle of the task calling tx_stop */

/* RX task state */
static TaskHandle_t s_rxTaskHandle       = NULL; /* handle of the running RX task */
static TaskHandle_t s_rxStopCallerHandle = NULL; /* handle of the task calling rx_stop */

/* App audio frame buffers */
static uint8_t s_txBuf[AUDIO_FRAME_SIZE];
static uint8_t s_rxBuf[AUDIO_FRAME_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/

void streamer_register_baresip_ausrc(baresip_ausrc_callback_h callback, void *arg)
{
    g_baresip_callbacks.ausrc_cb  = callback;
    g_baresip_callbacks.ausrc_arg = arg;
}

void streamer_register_baresip_auplay(baresip_auplay_callback_h callback, void *arg)
{
    g_baresip_callbacks.auplay_cb  = callback;
    g_baresip_callbacks.auplay_arg = arg;
}

static int streamer_network_read(uint8_t *buffer, uint32_t length)
{
    if (g_baresip_callbacks.auplay_cb)
    {
        g_baresip_callbacks.auplay_cb((int16_t *)buffer, length / 2, g_baresip_callbacks.auplay_arg);
    }
    else
    {
        /* Fill with silence if no callback registered */
        memset(buffer, 0, length);
    }

    return 0;
}

static int streamer_network_write(uint8_t *buffer, uint32_t length)
{
    if (g_baresip_callbacks.ausrc_cb)
    {
        g_baresip_callbacks.ausrc_cb((int16_t *)buffer, length / 2, g_baresip_callbacks.ausrc_arg);
    }

    return 0;
}

void STREAMER_pcm_init(void)
{
    streamer_pcm_init();
}

static void STREAMER_tx_task(void *arg)
{
    uint32_t notif;
    (void)arg;
    PRINTF("[STREAMER] TX task started\r\n");

    for (;;)
    {
        /* Need to stop ? */
        if (xTaskNotifyWait(0U, STREAMER_NOTIFY_STOP, &notif, 0U) == pdTRUE && (notif & STREAMER_NOTIFY_STOP) != 0U)
        {
            break;
        }

        streamer_network_read(s_txBuf, AUDIO_FRAME_SIZE);
        streamer_pcm_write(s_txBuf, AUDIO_FRAME_SIZE);

        if (streamer_pcm_tx_underrun_get_and_clear())
        {
            PRINTF("[STREAMER] TX DMA underrun detected\r\n");
        }
    }

    PRINTF("[STREAMER] TX task stopped\r\n");

    /* Notify the stop-caller */
    if (s_txStopCallerHandle != NULL)
        xTaskNotifyGive(s_txStopCallerHandle);

    s_txTaskHandle = NULL;
    vTaskDelete(NULL);
}

static void STREAMER_rx_task(void *arg)
{
    uint32_t notif;
    (void)arg;
    PRINTF("[STREAMER] RX task started\r\n");

    for (;;)
    {
        /* Need to stop ? */
        if (xTaskNotifyWait(0U, STREAMER_NOTIFY_STOP, &notif, 0U) == pdTRUE && (notif & STREAMER_NOTIFY_STOP) != 0U)
        {
            break;
        }

        if (streamer_pcm_read(s_rxBuf, AUDIO_FRAME_SIZE) == 0)
        {
            streamer_network_write(s_rxBuf, AUDIO_FRAME_SIZE);

            if (streamer_pcm_rx_overrun_get_and_clear())
            {
                PRINTF("[STREAMER] RX buffer overrun detected\r\n");
            }
        }
    }

    PRINTF("[STREAMER] RX task stopped\r\n");

    /* Notify the stop-caller */
    if (s_rxStopCallerHandle != NULL)
        xTaskNotifyGive(s_rxStopCallerHandle);

    s_rxTaskHandle = NULL;
    vTaskDelete(NULL);
}

/*******************************************************************************
 * Public API
 ******************************************************************************/

status_t STREAMER_tx_create(void)
{
    if (streamer_pcm_tx_open(0) != 0)
    {
        PRINTF("[STREAMER] TX open failed\r\n");
        return kStatus_Fail;
    }

    streamer_pcm_tx_setparams(DEMO_AUDIO_SAMPLE_RATE, DEMO_AUDIO_BIT_WIDTH, DEMO_MIC_CHANNEL_NUM, DEMO_VOLUME);

    if (xTaskCreate(STREAMER_tx_task, "StreamerTX", STREAMER_TX_TASK_STACK, NULL, STREAMER_TX_TASK_PRIO,
                    &s_txTaskHandle) != pdPASS)
    {
        PRINTF("[STREAMER] TX task create failed\r\n");
        streamer_pcm_tx_close();
        return kStatus_Fail;
    }

    return kStatus_Success;
}

void STREAMER_tx_stop(void)
{
    if (s_txTaskHandle == NULL)
        return;

    /* Record which task is waiting so the audio task can notify it back */
    s_txStopCallerHandle = xTaskGetCurrentTaskHandle();

    /* Notify audio task to stop and abort its blocking calls */
    xTaskNotify(s_txTaskHandle, STREAMER_NOTIFY_STOP, eSetBits);
    xTaskAbortDelay(s_txTaskHandle);

    /* Wait for confirmation */
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)) == 0U)
        PRINTF("[STREAMER] Warning: TX task did not exit within timeout\r\n");

    streamer_pcm_tx_close();

    s_txStopCallerHandle = NULL;
}

status_t STREAMER_rx_create(void)
{
    if (streamer_pcm_rx_open(0) != 0)
    {
        PRINTF("[STREAMER] RX open failed\r\n");
        return kStatus_Fail;
    }

    streamer_pcm_rx_setparams(DEMO_AUDIO_SAMPLE_RATE, DEMO_AUDIO_BIT_WIDTH, DEMO_MIC_CHANNEL_NUM);

    if (xTaskCreate(STREAMER_rx_task, "StreamerRX", STREAMER_RX_TASK_STACK, NULL, STREAMER_RX_TASK_PRIO,
                    &s_rxTaskHandle) != pdPASS)
    {
        PRINTF("[STREAMER] RX task create failed\r\n");
        streamer_pcm_rx_close();
        return kStatus_Fail;
    }

    return kStatus_Success;
}

void STREAMER_rx_stop(void)
{
    if (s_rxTaskHandle == NULL)
        return;

    /* Record which task is waiting so the audio task can notify it back */
    s_rxStopCallerHandle = xTaskGetCurrentTaskHandle();

    /* Notify audio task to stop and abort its blocking calls */
    xTaskNotify(s_rxTaskHandle, STREAMER_NOTIFY_STOP, eSetBits);
    xTaskAbortDelay(s_rxTaskHandle);

    /* Wait for confirmation */
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)) == 0U)
        PRINTF("[STREAMER] Warning: RX task did not exit within timeout\r\n");

    streamer_pcm_rx_close();

    s_rxStopCallerHandle = NULL;
}
