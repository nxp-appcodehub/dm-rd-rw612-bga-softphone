/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file softphone.c
 * @brief High-level softphone API implementation — wraps baresip_main.c.
 */

#include "softphone.h"
#include "fsl_debug_console.h"
#include <re.h>
#include <baresip.h>
#include <string.h>

/*******************************************************************************
 * External symbols provided by baresip_main.c
 ******************************************************************************/

extern struct ua        *app_ua;
extern struct call      *g_active_call;
extern struct call      *g_pending_call;

/* Declared in baresip_main.c – starts the FreeRTOS task */
extern void StartBaresipTask(void);

/*******************************************************************************
 * Internal callback registry
 ******************************************************************************/

static struct
{
    softphone_call_event_cb_t    call_event_cb;
    void                        *call_event_arg;

    softphone_incoming_call_cb_t incoming_call_cb;
    void                        *incoming_call_arg;

    softphone_message_cb_t       message_cb;
    void                        *message_arg;
} s_cb = {0};

/*******************************************************************************
 * Internal helpers (called from baresip_main.c)
 ******************************************************************************/

/**
 * Called by baresip_main.c's call_event_handler to forward events to the
 * application layer.
 */
void softphone_notify_call_event(softphone_call_event_t event,
                                 const char *peer,
                                 const char *info)
{
    if (s_cb.call_event_cb)
        s_cb.call_event_cb(event, peer, info, s_cb.call_event_arg);
}

/**
 * Called by baresip_main.c's event_handler when BEVENT_CALL_INCOMING fires,
 * *before* the call is auto-answered.  Returns true if the application wants
 * to handle the answer itself (preventing the default auto-answer).
 */
bool softphone_notify_incoming_call(const char *peer)
{
    if (s_cb.incoming_call_cb)
    {
        s_cb.incoming_call_cb(peer, s_cb.incoming_call_arg);
        return true; /* application is in control */
    }
    return false; /* fall back to auto-answer */
}

/**
 * Called by baresip_main.c's simple_message_recv_handler.
 */
void softphone_notify_message(const char *from, const char *body)
{
    if (s_cb.message_cb)
        s_cb.message_cb(from, body, s_cb.message_arg);
}

/*******************************************************************************
 * Public API — Lifecycle
 ******************************************************************************/

void softphone_start(void)
{
    StartBaresipTask();
}

/*******************************************************************************
 * Public API — Call control
 ******************************************************************************/

void softphone_call(const char *uri)
{
    if (!uri || uri[0] == '\0')
    {
        PRINTF("[softphone] softphone_call: invalid URI\r\n");
        return;
    }

    /* baresip_make_call is defined in baresip_main.c */
    extern void baresip_make_call(const char *uri);
    baresip_make_call(uri);
}

void softphone_answer(void)
{
    if (!app_ua)
    {
        PRINTF("[softphone] softphone_answer: no UA available\r\n");
        return;
    }

    if (!g_pending_call)
    {
        PRINTF("[softphone] softphone_answer: no pending incoming call\r\n");
        return;
    }

    re_thread_enter();

    int err = ua_answer(app_ua, g_pending_call, VIDMODE_OFF);
    if (err)
    {
        PRINTF("[softphone] softphone_answer: ua_answer failed (%d)\r\n", err);
        call_hangup(g_pending_call, 500, "Server Error");
    }
    else
    {
        g_active_call   = g_pending_call;
        g_pending_call  = NULL;
        PRINTF("[softphone] call answered\r\n");
    }

    re_thread_leave();
}

void softphone_hangup(void)
{
    extern void baresip_hangup_call(void);

    /* Hang up whichever call is live */
    if (g_active_call)
    {
        baresip_hangup_call();
        return;
    }

    if (g_pending_call)
    {
        re_thread_enter();
        call_hangup(g_pending_call, 486, "Busy Here");
        mem_deref(g_pending_call);
        g_pending_call = NULL;
        re_thread_leave();
        return;
    }

    re_thread_leave();
    PRINTF("[softphone] softphone_hangup: no active call\r\n");
}

/*******************************************************************************
 * Public API — Messaging
 ******************************************************************************/

int softphone_send_message(const char *uri, const char *message)
{
    int err;

    if (!app_ua)
    {
        PRINTF("[softphone] softphone_send_message: no UA available\r\n");
        return EINVAL;
    }

    if (!uri || !message)
        return EINVAL;

    re_thread_enter();
    err = message_send(app_ua, uri, message, NULL, NULL);
    re_thread_leave();

    if (err)
        PRINTF("[softphone] softphone_send_message: message_send failed (%d)\r\n", err);

    return err;
}


bool softphone_get_peer_ip(char *ip, size_t size)
{
    struct sa addr;

    if (!g_pending_call || !ip)
        return false;

    if (call_msg_src(g_pending_call, &addr))
        return false;

    sa_ntop(&addr, ip, size);

    return true;
}


/*******************************************************************************
 * Public API — Callback registration
 ******************************************************************************/

void softphone_register_incoming_call_cb(softphone_incoming_call_cb_t cb,
                                         void *arg)
{
    s_cb.incoming_call_cb  = cb;
    s_cb.incoming_call_arg = arg;
}

void softphone_register_call_event_cb(softphone_call_event_cb_t cb, void *arg)
{
    s_cb.call_event_cb  = cb;
    s_cb.call_event_arg = arg;
}

void softphone_register_message_cb(softphone_message_cb_t cb, void *arg)
{
    s_cb.message_cb  = cb;
    s_cb.message_arg = arg;
}
