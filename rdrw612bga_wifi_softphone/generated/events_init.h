/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


#ifndef EVENTS_INIT_H_
#define EVENTS_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"
typedef struct{
    char text[128];
    bool is_sender;
} chat_message_t;


#define MAX_CHAT_MESSAGES 4

extern chat_message_t chat_history[MAX_CHAT_MESSAGES];
extern uint8_t message_count;


void events_init(lv_ui *ui);

void events_init_home_screen(lv_ui *ui);
void events_init_incoming_call_screen(lv_ui *ui);
void events_init_keypad_screen(lv_ui *ui);
void events_init_call_screen(lv_ui *ui);
void events_init_settings_screen(lv_ui *ui);
void events_init_message_screen(lv_ui *ui);

void display_message(const char *msg, bool is_sender);
void refresh_chat_display(void);
void lvgl_show_home_screen(void *param);
void lvgl_show_incoming_call(void *param);
void lvgl_show_incoming_message(void *param);
const char *get_local_ip(void);
bool build_uri(const char *peer, const char *ip, char *sip_prefix, size_t sip_prefix_size, char *uri, size_t uri_size);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
