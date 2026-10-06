/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file softphone.h
 * @brief High-level softphone API wrapping baresip for FreeRTOS / MCUXpresso SDK.
 *
 * Usage:
 *   1. Register any desired callbacks (softphone_register_*).
 *   2. Call softphone_start() once the network is up.
 *   3. Use softphone_call(), softphone_answer(), softphone_hangup(),
 *      softphone_send_message() as needed.
 */

#ifndef SOFTPHONE_H
#define SOFTPHONE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Types
 ******************************************************************************/

/**
 * @brief Call event codes forwarded to the application.
 */
typedef enum
{
    SOFTPHONE_CALL_INCOMING,        /**< New incoming call (not yet answered)  */
    SOFTPHONE_CALL_OUTGOING,        /**< Outgoing call initiated               */
    SOFTPHONE_CALL_RINGING,         /**< Remote end is ringing                 */
    SOFTPHONE_CALL_PROGRESS,        /**< Early media / progress                */
    SOFTPHONE_CALL_ANSWERED,        /**< Call was answered                     */
    SOFTPHONE_CALL_ESTABLISHED,     /**< Media path established, RTP flowing   */
    SOFTPHONE_CALL_CLOSED,          /**< Call ended (any reason)               */
    SOFTPHONE_CALL_TRANSFER,        /**< Call transfer requested               */
    SOFTPHONE_CALL_TRANSFER_FAILED, /**< Call transfer failed                  */
    SOFTPHONE_CALL_MENC,            /**< Media encryption event                */
} softphone_call_event_t;

/**
 * @brief Callback invoked for every call state-change event.
 *
 * @param event   The call event.
 * @param peer    Null-terminated SIP URI of the remote party, or NULL.
 * @param info    Optional human-readable string associated with the event.
 * @param arg     User-provided argument supplied at registration time.
 */
typedef void (*softphone_call_event_cb_t)(softphone_call_event_t event,
                                          const char *peer,
                                          const char *info,
                                          void *arg);

/**
 * @brief Callback invoked when an incoming call arrives (before it is answered).
 *
 * The application may call softphone_answer() or softphone_hangup() from within
 * this callback.
 *
 * @param peer  Null-terminated SIP URI of the caller.
 * @param arg   User-provided argument supplied at registration time.
 */
typedef void (*softphone_incoming_call_cb_t)(const char *peer, void *arg);

/**
 * @brief Callback invoked when an incoming SIP MESSAGE is received.
 *
 * @param from    Null-terminated SIP URI of the sender.
 * @param message Null-terminated message body.
 * @param arg     User-provided argument supplied at registration time.
 */
typedef void (*softphone_message_cb_t)(const char *from,
                                       const char *message,
                                       void *arg);

/*******************************************************************************
 * Lifecycle
 ******************************************************************************/

/**
 * @brief Start the softphone (baresip) FreeRTOS task.
 *
 * Must be called after the network interface is up.  All callbacks should be
 * registered before calling this function.
 */
void softphone_start(void);

/*******************************************************************************
 * Call control
 ******************************************************************************/

/**
 * @brief Initiate an outgoing call.
 *
 * @param uri  SIP URI of the callee, e.g. "sip:1001@192.168.1.1".
 */
void softphone_call(const char *uri);

/**
 * @brief Answer the current incoming call.
 *
 * Has no effect if there is no pending incoming call.
 */
void softphone_answer(void);

/**
 * @brief Hang up the active or incoming call.
 *
 * Has no effect if there is no active call.
 */
void softphone_hangup(void);

/*******************************************************************************
 * Messaging
 ******************************************************************************/

/**
 * @brief Send a SIP MESSAGE (instant message) to a URI.
 *
 * @param uri     Destination SIP URI.
 * @param message Null-terminated message body (UTF-8).
 * @return 0 on success, non-zero errno on failure.
 */
int softphone_send_message(const char *uri, const char *message);

/*******************************************************************************
 * Callback registration
 ******************************************************************************/

/**
 * @brief Register a callback for incoming call notifications.
 *
 * Only one callback can be registered at a time; a subsequent call replaces
 * the previous registration.
 *
 * @param cb   Callback function pointer (NULL to unregister).
 * @param arg  Opaque argument passed to the callback.
 */
void softphone_register_incoming_call_cb(softphone_incoming_call_cb_t cb,
                                         void *arg);

/**
 * @brief Register a callback for call state-change events.
 *
 * Only one callback can be registered at a time; a subsequent call replaces
 * the previous registration.
 *
 * @param cb   Callback function pointer (NULL to unregister).
 * @param arg  Opaque argument passed to the callback.
 */
void softphone_register_call_event_cb(softphone_call_event_cb_t cb,
                                      void *arg);

/**
 * @brief Register a callback for incoming SIP MESSAGE notifications.
 *
 * Only one callback can be registered at a time; a subsequent call replaces
 * the previous registration.
 *
 * @param cb   Callback function pointer (NULL to unregister).
 * @param arg  Opaque argument passed to the callback.
 */
void softphone_register_message_cb(softphone_message_cb_t cb, void *arg);


bool softphone_get_peer_ip(char *ip, size_t size);

/*******************************************************************************
 * Internal notification hooks (called by baresip_main.c — not for app use)
 ******************************************************************************/

void softphone_notify_call_event(softphone_call_event_t event,
                                 const char *peer,
                                 const char *info);

bool softphone_notify_incoming_call(const char *peer);

void softphone_notify_message(const char *from, const char *body);

#ifdef __cplusplus
}
#endif

#endif /* SOFTPHONE_H */
