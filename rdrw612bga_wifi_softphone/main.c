/** @file main.c
 *
 *  @brief main file
 *
 *  Copyright 2022-2023, 2025 NXP
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 */

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////

#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_shell.h"
#include "shell_task.h"
#include "app.h"
#include "cred_flash_storage.h"
#include "softphone.h"
#include "wlan_wrpr.h"


#ifndef CONFIG_CLI_ONLY
#include "lvgl_support.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
#include "lvgl_demo_utils.h"
#endif /* CONFIG_CLI_ONLY */

#include <string.h>
#include <stdio.h>

#include <osa.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define main_task_PRIORITY          1
#define main_task_STACK_DEPTH       800
#define DEMO_WIFI_LABEL             "MyWifi"
#define CREDENTIALS_FILE            "wifi_cred.dat"


static void sanitize_message(const char *sip_message, char *output, size_t output_size);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static volatile bool wlan_connected = false;

#ifndef CONFIG_CLI_ONLY
static volatile bool s_lvgl_initialized = false;
lv_ui guider_ui;
#endif /* CONFIG_CLI_ONLY */

static char incoming_peer[64];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#ifndef CONFIG_CLI_ONLY
void print_cb(lv_log_level_t level, const char * buf);

#if LV_USE_LOG
void print_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);

    PRINTF("\r%s\n", buf);
}
#endif
#endif /* CONFIG_CLI_ONLY */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* WiFi commands */
static shell_status_t cmd_connect(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_connect_saved(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_scan(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_disconnect(void *shellHandle, int32_t argc, char **argv);

/* UAP commands */
static shell_status_t cmd_uap_start(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_uap_stop(void *shellHandle, int32_t argc, char **argv);

/* Debug commands */
static shell_status_t cmd_task_stats(void *shellHandle, int32_t argc, char **argv);

/* Softphone commands */
static shell_status_t cmd_sip_start(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_sip_call(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_sip_answer(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_sip_hangup(void *shellHandle, int32_t argc, char **argv);
static shell_status_t cmd_sip_send_msg(void *shellHandle, int32_t argc, char **argv);

/* Softphone callbacks */
static void on_incoming_call(const char *peer, void *arg);
static void on_call_event(softphone_call_event_t event, const char *peer, const char *info, void *arg);
static void on_message_received(const char *from, const char *message, void *arg);


#ifndef CONFIG_CLI_ONLY
static void lvgl_tick_hook(void);
#endif /* CONFIG_CLI_ONLY */

/*******************************************************************************
 * Code
 ******************************************************************************/

SHELL_COMMAND_DEFINE(task_stats,
                     "\r\n\"task_stats\":\r\n"
                     "   Print FreeRTOS task CPU runtime stats and stack high-water marks.\r\n",
                     cmd_task_stats,
                     0);

SHELL_COMMAND_DEFINE(wlan_scan, "\r\n\"wlan_scan\": Scans networks.\r\n", cmd_scan, 0);

SHELL_COMMAND_DEFINE(wlan_start_ap,
                     "\r\n\"wlan_start_ap\":\r\n"
                     "   Start the Wi-Fi micro access point (UAP).\r\n"
                     "   SSID: rw612_softphone  Password: 12345678\r\n"
                     "   The board will act as an access point on 192.168.1.1.\r\n",
                     cmd_uap_start,
                     0);

SHELL_COMMAND_DEFINE(wlan_stop_ap,
                     "\r\n\"wlan_stop_ap\":\r\n"
                     "   Stop the Wi-Fi micro access point (UAP).\r\n",
                     cmd_uap_stop,
                     0);

SHELL_COMMAND_DEFINE(wlan_connect_with_password,
                     "\r\n\"wlan_connect_with_password ssid password\":\r\n"
                     "   Connects to the specified network with password.\r\n"
                     " Usage:\r\n"
                     "   ssid:        network SSID\r\n"
                     "   password:    password\r\n",
                     cmd_connect,
                     2);

SHELL_COMMAND_DEFINE(wlan_connect,
                     "\r\n\"wlan_connect ssid\":\r\n"
                     "   Connects to the specified network without password.\r\n"
                     " Usage:\r\n"
                     "   ssid:        network SSID\r\n",
                     cmd_connect,
                     1);

SHELL_COMMAND_DEFINE(wlan_disconnect,
                     "\r\n\"wlan_disconnect\":\r\n"
                     "   Disconnect from connected network\r\n",
                     cmd_disconnect,
                     0);

SHELL_COMMAND_DEFINE(wlan_connect_saved,
                     "\r\n\"wlan_connect_saved\":\r\n"
                     "   Connects to the last saved network credentials stored in flash.\r\n",
                     cmd_connect_saved,
                     0);

SHELL_COMMAND_DEFINE(sip_start,
                     "\r\n\"sip_start\":\r\n"
                     "   Start the softphone (baresip) task.\r\n"
                     "   Must be called after WiFi is connected.\r\n",
                     cmd_sip_start,
                     0);

SHELL_COMMAND_DEFINE(sip_call,
                     "\r\n\"sip_call <uri>\":\r\n"
                     "   Initiate an outgoing SIP call.\r\n"
                     " Usage:\r\n"
                     "   uri:  SIP URI of the callee, e.g. sip:1001@192.168.1.1\r\n",
                     cmd_sip_call,
                     1);

SHELL_COMMAND_DEFINE(sip_answer,
                     "\r\n\"sip_answer\":\r\n"
                     "   Answer the current incoming SIP call.\r\n",
                     cmd_sip_answer,
                     0);

SHELL_COMMAND_DEFINE(sip_hangup,
                     "\r\n\"sip_hangup\":\r\n"
                     "   Hang up the active or incoming SIP call.\r\n",
                     cmd_sip_hangup,
                     0);

SHELL_COMMAND_DEFINE(sip_send_msg,
                     "\r\n\"sip_send_msg <uri> <message>\":\r\n"
                     "   Send a SIP instant message.\r\n"
                     " Usage:\r\n"
                     "   uri:     Destination SIP URI\r\n"
                     "   message: Message text (one argument, use quotes if needed)\r\n",
                     cmd_sip_send_msg,
                     2);

static void printSeparator(void)
{
    PRINTF("========================================\r\n");
}

/* Link lost callback */
static void LinkStatusChangeCallback(bool linkState)
{
    if (linkState == false)
    {
        PRINTF("-------- LINK LOST --------\r\n");
    }
    else
    {
        PRINTF("-------- LINK REESTABLISHED --------\r\n");
    }
}


#ifndef CONFIG_CLI_ONLY
static void AppTask(void *param)
{
    OSA_SetupTickFunction(lvgl_tick_hook);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

#if LV_USE_LOG
    lv_log_register_print_cb(print_cb);
#endif

    LV_LOG("lvgl guider demo started\r\n");

    s_lvgl_initialized = true;

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);

    for (;;)
    {
        vTaskDelay(lv_timer_handler());
    }
}
#endif /* CONFIG_CLI_ONLY */

void task_main(void *param)
{
    int err = 0;
    BaseType_t stat=0;

    static shell_command_t *wifi_commands[] = {
        SHELL_COMMAND(wlan_scan), SHELL_COMMAND(wlan_connect), SHELL_COMMAND(wlan_connect_with_password),
        SHELL_COMMAND(wlan_disconnect), SHELL_COMMAND(wlan_connect_saved),
        SHELL_COMMAND(wlan_start_ap), SHELL_COMMAND(wlan_stop_ap),
        SHELL_COMMAND(sip_start), SHELL_COMMAND(sip_call), SHELL_COMMAND(sip_answer),
        SHELL_COMMAND(sip_hangup), SHELL_COMMAND(sip_send_msg),
        SHELL_COMMAND(task_stats),
        NULL // end of list
    };

    PRINTF("Initialize flash storage\r\n");
    printSeparator();
    if (init_flash_storage(CREDENTIALS_FILE) != 0)
    {
        PRINTF("[!] Failed to initialize flash storage\r\n");
    }

    /* Register softphone callbacks */
    // softphone_register_incoming_call_cb(on_incoming_call, NULL);
    // softphone_register_call_event_cb(on_call_event, NULL);
    // softphone_register_message_cb(on_message_received, NULL);

    PRINTF("Initialize WLAN \r\n");
    printSeparator();

    err = wlan_wrpr_init(LinkStatusChangeCallback);
    if (err != 0) {
        PRINTF("[!] wlan_wrpr_init: Failed, error: %d\r\n", (uint32_t)err);
        while (true)
        {
            ;
        }
    }

    if(wlan_wrpr_ssid[0] != '\0') {
        PRINTF("Network in storage: %s. With password: %s\r\n", wlan_wrpr_ssid, wlan_wrpr_passphrase);
    }
    else {
        PRINTF("No network stored.\r\n");
    }

    /* Register the Softphone event callbacks */
    softphone_register_incoming_call_cb(on_incoming_call, NULL);
    softphone_register_call_event_cb(on_call_event, NULL);
    softphone_register_message_cb(on_message_received, NULL);

#ifndef CONFIG_CLI_ONLY
    /* Start the LVGL task now that the Wi-Fi and Storage is enabled and initialized. */
    stat = xTaskCreate(AppTask, "lvgl", 1024 * 8, NULL, main_task_PRIORITY , NULL);
    if (pdPASS != stat)
    {
        PRINTF("Failed to create lvgl task");
        while (1)
            ;
    }
#else
    (void)stat;
#endif /* CONFIG_CLI_ONLY */
    PRINTF("Initialize CLI\r\n");
    printSeparator();
    shell_task_init(wifi_commands);

    vTaskDelete(NULL);
}

static shell_status_t cmd_connect(void *shellHandle, int32_t argc, char **argv)
{
    int err;
    if (wlan_wrpr_connected == true)
    {
        SHELL_Printf(shellHandle, "Leave network before connecting to a new one!\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Connecting to network: %s\r\n", argv[1]);
    if (argc < 3)
        err = wlan_wrpr_connect(argv[1], "");
    else
        err = wlan_wrpr_connect(argv[1], argv[2]);

    if (err != 0)
    {
        SHELL_Printf(shellHandle, "Failed to connect to network!\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Network joined. Wi-Fi stored to flash\r\n");

    return kStatus_SHELL_Success;
}

/*******************************************************************************
 * Softphone callbacks
 ******************************************************************************/

static void on_incoming_call(const char *peer, void *arg)
{
    (void)arg;
    PRINTF("\r\n");
    PRINTF("========================================\r\n");
    PRINTF("  INCOMING CALL from: %s\r\n", peer ? peer : "unknown");
    PRINTF("  Type 'sip_answer' to answer\r\n");
    PRINTF("  Type 'sip_hangup' to reject\r\n");
    PRINTF("========================================\r\n");

    snprintf(incoming_peer, sizeof(incoming_peer), "%s", peer ? peer : "unknown");

#ifndef CONFIG_CLI_ONLY
    lv_async_call(lvgl_show_incoming_call, incoming_peer);
#endif /* CONFIG_CLI_ONLY */
}

static void on_call_event(softphone_call_event_t event,
                          const char *peer,
                          const char *info,
                          void *arg)
{
    (void)arg;
    const char *peer_str = peer ? peer : "unknown";

    switch (event)
    {
        case SOFTPHONE_CALL_INCOMING:
            PRINTF("[SIP] Incoming call from %s\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_OUTGOING:
            PRINTF("[SIP] Outgoing call to %s\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_RINGING:
            PRINTF("[SIP] Ringing ... (%s)\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_PROGRESS:
            PRINTF("[SIP] Call progress (%s)\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_ANSWERED:
            PRINTF("[SIP] Call answered (%s)\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_ESTABLISHED:
            PRINTF("[SIP] Call established with %s — audio active\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_CLOSED:
            PRINTF("[SIP] Call closed (%s)\r\n", peer_str);
#ifndef CONFIG_CLI_ONLY
            lv_async_call(lvgl_show_home_screen, NULL);
#endif /* CONFIG_CLI_ONLY */
            break;
        case SOFTPHONE_CALL_TRANSFER:
            PRINTF("[SIP] Call transfer requested (%s)\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_TRANSFER_FAILED:
            PRINTF("[SIP] Call transfer failed (%s)\r\n", peer_str);
            break;
        case SOFTPHONE_CALL_MENC:
            PRINTF("[SIP] Media encryption event (%s): %s\r\n", peer_str, info ? info : "");
            break;
        default:
            PRINTF("[SIP] Unknown call event %d (%s)\r\n", (int)event, peer_str);
            break;
    }
}

static void on_message_received(const char *from, const char *message, void *arg)
{
    (void)arg;

    char clean_msg[256] = {0};

    sanitize_message(message, clean_msg, sizeof(clean_msg));

    PRINTF("\r\n");
    PRINTF("========================================\r\n");
    PRINTF("  SIP MESSAGE from: %s\r\n", from ? from : "unknown");
    PRINTF("  %s\r\n", clean_msg);
    PRINTF("========================================\r\n");

#ifndef CONFIG_CLI_ONLY
    display_message(clean_msg, false);
#endif /* CONFIG_CLI_ONLY */

    snprintf(incoming_peer, sizeof(incoming_peer), "%s", from ? from : "unknown");
    lv_async_call(lvgl_show_incoming_message, incoming_peer);
}

/*
 * Extract the message body from a raw SIP request by reading the
 * Content-Length header and copying exactly that many bytes from the
 * body that starts after the blank line (\r\n\r\n) separator.
 *
 * If Content-Length is missing or the body is shorter than declared,
 * only the available bytes are copied.  The result is always
 * null-terminated.
 */
static void sanitize_message(const char *sip_message, char *output, size_t output_size)
{
    if ((sip_message == NULL) || (output == NULL) || (output_size == 0))
    {
        return;
    }

    output[0] = '\0';

    /* Locate the blank line that separates headers from body. */
    const char *body_start = strstr(sip_message, "\r\n\r\n");
    if (body_start == NULL)
    {
        /* No header/body separator found - treat the whole input as body. */
        strncpy(output, sip_message, output_size - 1);
        output[output_size - 1] = '\0';
        return;
    }
    body_start += 4; /* skip the \r\n\r\n */

    /* Find Content-Length header (case-insensitive prefix match). */
    size_t content_len = 0;
    const char *cl = sip_message;
    while (cl < body_start)
    {
        if (strncasecmp(cl, "Content-Length:", 15) == 0)
        {
            /* Skip optional whitespace after the colon. */
            const char *val = cl + 15;
            while (*val == ' ' || *val == '\t')
            {
                val++;
            }
            content_len = (size_t)strtoul(val, NULL, 10);
            break;
        }
        /* Advance to the next header line. */
        const char *eol = strstr(cl, "\r\n");
        if (eol == NULL)
        {
            break;
        }
        cl = eol + 2;
    }

    /* Copy exactly content_len bytes, clamped to the output buffer. */
    size_t copy_len = content_len;
    if (copy_len >= output_size)
    {
        copy_len = output_size - 1;
    }

    /* Also clamp to what is actually available in the buffer. */
    size_t available = strlen(body_start);
    if (copy_len > available)
    {
        copy_len = available;
    }

    memcpy(output, body_start, copy_len);
    output[copy_len] = '\0';
}


/*******************************************************************************
 * Softphone shell commands
 ******************************************************************************/

static shell_status_t cmd_sip_start(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    (void)argv;
    SHELL_Printf(shellHandle, "Starting softphone task...\r\n");
    softphone_start();
    return kStatus_SHELL_Success;
}

static shell_status_t cmd_sip_call(void *shellHandle, int32_t argc, char **argv)
{
    SHELL_Printf(shellHandle, "Calling: %s\r\n", argv[1]);
    softphone_call(argv[1]);
    return kStatus_SHELL_Success;
}

static shell_status_t cmd_sip_answer(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    (void)argv;
    SHELL_Printf(shellHandle, "Answering call...\r\n");
    softphone_answer();
    return kStatus_SHELL_Success;
}

static shell_status_t cmd_sip_hangup(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    (void)argv;
    SHELL_Printf(shellHandle, "Hanging up...\r\n");
    softphone_hangup();
    return kStatus_SHELL_Success;
}

static shell_status_t cmd_sip_send_msg(void *shellHandle, int32_t argc, char **argv)
{
    int err = softphone_send_message(argv[1], argv[2]);
    if (err != 0)
        SHELL_Printf(shellHandle, "Failed to send message (err=%d)\r\n", err);
    else
        SHELL_Printf(shellHandle, "Message sent to %s\r\n", argv[1]);
    return kStatus_SHELL_Success;
}

/*******************************************************************************
 * UAP shell commands
 ******************************************************************************/

static shell_status_t cmd_uap_start(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    (void)argv;

    SHELL_Printf(shellHandle, "Starting access point (SSID: rw612_softphone) ...\r\n");

    if (wlan_wrpr_start_uap() != 0)
    {
        SHELL_Printf(shellHandle, "[!] Failed to start access point\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Access point started. IP: 192.168.1.1\r\n");
    return kStatus_SHELL_Success;
}

static shell_status_t cmd_uap_stop(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    (void)argv;

    SHELL_Printf(shellHandle, "Stopping access point ...\r\n");

    if (wlan_wrpr_stop_uap() != 0)
    {
        SHELL_Printf(shellHandle, "[!] Failed to stop access point\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Access point stopped\r\n");
    return kStatus_SHELL_Success;
}

/*******************************************************************************
 * WiFi shell commands
 ******************************************************************************/

static shell_status_t cmd_connect_saved(void *shellHandle, int32_t argc, char **argv)
{
    int err;

    if (wlan_wrpr_connected == true)
    {
        SHELL_Printf(shellHandle, "Leave network before connecting to a new one!\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Connecting to saved network\r\n");

    err = wlan_wrpr_connect_saved();
    if (err != 0)
    {
        SHELL_Printf(shellHandle, "Failed to connect to saved network!\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Network %s joined\r\n", wlan_wrpr_ssid);

    return kStatus_SHELL_Success;
}

static shell_status_t cmd_disconnect(void *shellHandle, int32_t argc, char **argv)
{
    if (wlan_wrpr_connected == false)
    {
        SHELL_Printf(shellHandle, "No network connected!\r\n");
        return kStatus_SHELL_Success;
    }

    if (wlan_wrpr_disconnect() != 0)
    {
        SHELL_Printf(shellHandle, "Failed to leave the network!\r\n");
        return kStatus_SHELL_Success;
    }

    SHELL_Printf(shellHandle, "Disconnected from network\r\n");

    return kStatus_SHELL_Success;
}

/*******************************************************************************
 * Debug shell commands
 ******************************************************************************/

/* Buffer sized for up to 20 tasks × ~50 chars each */
#define TASK_STATS_BUF_SIZE  1200U

static shell_status_t cmd_task_stats(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    (void)argv;

    static char buf[TASK_STATS_BUF_SIZE];

    /* --- CPU runtime stats --- */
    SHELL_Printf(shellHandle, "\r\n--- Task CPU Runtime Stats ---\r\n");
    SHELL_Printf(shellHandle, "%-20s %12s %6s\r\n", "Task Name", "Abs Time", "%%");
    SHELL_Printf(shellHandle, "--------------------------------------------\r\n");
    vTaskGetRunTimeStats(buf);
    SHELL_Printf(shellHandle, "%s", buf);

    /* --- Stack high-water marks --- */
    SHELL_Printf(shellHandle, "\r\n--- Task Stack High-Water Marks ---\r\n");
    SHELL_Printf(shellHandle, "%-20s %10s\r\n", "Task Name", "HWM (words)");
    SHELL_Printf(shellHandle, "-------------------------------\r\n");

    TaskStatus_t *task_array;
    UBaseType_t task_count = uxTaskGetNumberOfTasks();
    task_array = pvPortMalloc(task_count * sizeof(TaskStatus_t));
    if (task_array != NULL)
    {
        uint32_t total_runtime;
        task_count = uxTaskGetSystemState(task_array, task_count, &total_runtime);
        for (UBaseType_t i = 0; i < task_count; i++)
        {
            SHELL_Printf(shellHandle, "%-20s %10u\r\n",
                         task_array[i].pcTaskName,
                         (unsigned)task_array[i].usStackHighWaterMark);
        }
        vPortFree(task_array);
    }
    else
    {
        SHELL_Printf(shellHandle, "[!] Failed to allocate task status buffer\r\n");
    }

    /* --- Free heap --- */
    SHELL_Printf(shellHandle, "\r\nFree heap: %u bytes\r\n", (unsigned)xPortGetFreeHeapSize());
    SHELL_Printf(shellHandle, "Min free heap ever: %u bytes\r\n", (unsigned)xPortGetMinimumEverFreeHeapSize());

    return kStatus_SHELL_Success;
}

static shell_status_t cmd_scan(void *shellHandle, int32_t argc, char **argv)
{
    (void)argc;
    char *scanData = NULL;

    SHELL_Printf(shellHandle, "\r\nInitiating scan...\r\n");
    scanData = wlan_wrpr_scan();
    if (scanData == NULL)
    {
        SHELL_Printf(shellHandle, "Error while scanning!\r\n");
    }
    else
    {
        vPortFree(scanData);
    }

#if (defined(SDK_DEBUGCONSOLE) && (SDK_DEBUGCONSOLE == DEBUGCONSOLE_REDIRECT_TO_SDK))
    /*
     * Scanning prints the found networks to the console.
     * Wait for debug console output to be printed before returning from
     * the command, otherwise shell prompt could be printed in
     * the middle of the output of the network scan.
     */
    (void)DbgConsole_Flush();
#endif

    return kStatus_SHELL_Success;
}

int main(void)
{
    BaseType_t result = 0;
    BaseType_t stat=0;
    (void)result;

    BOARD_InitHardware();
#ifndef CONFIG_CLI_ONLY
    DEMO_InitUsTimer();
#endif

    printSeparator();

    result = xTaskCreate(task_main, "main", main_task_STACK_DEPTH, NULL, main_task_PRIORITY, NULL);

    assert(pdPASS == result);

    vTaskStartScheduler();
    for (;;)
        ;
}


/*!
 * @brief Malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
    PRINTF("Malloc failed. Increase the heap size.");

    for (;;)
        ;
}

/*!
 * @brief FreeRTOS tick hook.
 */

#ifndef CONFIG_CLI_ONLY
static void lvgl_tick_hook(void)
{
    if (s_lvgl_initialized)
    {
        lv_tick_inc(1);
    }
}
#endif /* CONFIG_CLI_ONLY */

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    for (;;)
        ;
}


