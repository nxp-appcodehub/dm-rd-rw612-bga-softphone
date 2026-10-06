/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "softphone.h"
#include "wlan_wrpr.h"
#include "wpl.h"
#include "lwip/inet.h"
#include "lwip/netif.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


#include <string.h>
#include <stdio.h>
#include <wifi_manager.h>
#include "fsl_debug_console.h"

#define MAX_DIGITS 15


static char phone_number[MAX_DIGITS] = "";

chat_message_t chat_history[MAX_CHAT_MESSAGES];
uint8_t message_count = 0;
char sip_uri[128];
char sip_prefix[128];
bool incomming_call_flag = false;
bool incomming_message_flag = false;

static void scan_networks(void);

static void home_screen_call_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        PRINTF("Calling: %s\n", sip_uri);
        softphone_call(sip_uri);
        lv_label_set_text(guider_ui.call_screen_num_label, sip_uri);
        ui_load_scr_animation(&guider_ui, &guider_ui.call_screen, guider_ui.call_screen_del, &guider_ui.home_screen_del, setup_scr_call_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}


static void home_screen_keypad_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.keypad_screen, guider_ui.keypad_screen_del, &guider_ui.home_screen_del, setup_scr_keypad_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}
static void home_screen_chat_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.message_screen, guider_ui.message_screen_del, &guider_ui.home_screen_del, setup_scr_message_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}

void events_init_home_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->home_screen_call_imgbtn, home_screen_call_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_screen_keypad_imgbtn, home_screen_keypad_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_screen_chat_imgbtn, home_screen_chat_imgbtn_event_handler, LV_EVENT_ALL, ui);
}

static void incoming_call_screen_reject_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        softphone_hangup();
        ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.incoming_call_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}

static void incoming_call_screen_answer_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {

        softphone_answer();
        if(!incomming_call_flag){
            lv_label_set_text(guider_ui.call_screen_num_label, phone_number);
        }
        incomming_call_flag = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.call_screen, guider_ui.call_screen_del, &guider_ui.incoming_call_screen_del, setup_scr_call_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}


void events_init_incoming_call_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->incoming_call_screen_reject_imgbtn, incoming_call_screen_reject_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->incoming_call_screen_answer_imgbtn, incoming_call_screen_answer_imgbtn_event_handler, LV_EVENT_ALL, ui);
}


static void keypad_screen_keypad_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        const char * txt = lv_buttonmatrix_get_button_text(guider_ui.keypad_screen_keypad, id);

        if(txt == NULL) return;

        if(strcmp(txt, "DEL") == 0)
        {
            size_t len = strlen(phone_number);
            if(len > 0)
            {
                phone_number[len - 1] = '\0';
            }
        }
        else
        {
            if(strlen(phone_number) < MAX_DIGITS)
            {
                strcat(phone_number, txt);
            }
        }

        lv_label_set_text(guider_ui.keypad_screen_num_label, phone_number);

        break;
    }
    default:
        break;
    }
}

static void keypad_screen_call_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {


        snprintf(sip_uri, sizeof(sip_uri), "%s%s", sip_prefix, phone_number);

        lv_label_set_text(guider_ui.home_screen_last_num_label, sip_uri);
        lv_label_set_text(guider_ui.call_screen_num_label, sip_uri);

        PRINTF("Calling: %s\n", sip_uri);
        softphone_call(sip_uri);
        ui_load_scr_animation(&guider_ui, &guider_ui.call_screen, guider_ui.call_screen_del, &guider_ui.keypad_screen_del, setup_scr_call_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}

static void keypad_screen_back_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.keypad_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}

void events_init_keypad_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->keypad_screen_keypad, keypad_screen_keypad_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->keypad_screen_call_imgbtn, keypad_screen_call_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->keypad_screen_back_imgbtn, keypad_screen_back_imgbtn_event_handler, LV_EVENT_ALL, ui);
}

static void call_screen_endcall_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        softphone_hangup();
        ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.call_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}

void events_init_call_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->call_screen_endcall_imgbtn, call_screen_endcall_imgbtn_event_handler, LV_EVENT_ALL, ui);
}



static void settings_screen_connect_btn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {

        lv_obj_clear_flag(guider_ui.settings_screen_connecting_label, LV_OBJ_FLAG_HIDDEN);
        lv_refr_now(NULL);


        char ssid_roller[64];
        lv_roller_get_selected_str(guider_ui.settings_screen_wifi_roller,ssid_roller,sizeof(ssid_roller));

        const char *password = lv_textarea_get_text(guider_ui.settings_screen_password_keyboard);

        char uri_0001[64];

        int ret = wlan_wrpr_connect(ssid_roller, password);

        if(ret == 0)
        {
            softphone_start();
            snprintf(uri_0001, sizeof(uri_0001), "sip:0001@%s", get_local_ip());
            lv_label_set_text(guider_ui.home_screen_ext_num_label, uri_0001);
            lv_obj_set_style_image_recolor(guider_ui.call_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.home_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.incoming_call_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.keypad_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.message_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.settings_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        }
    }
    default:
        break;
    }
}

static void settings_screen_scan_btn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {

        lv_obj_clear_flag(guider_ui.settings_screen_connecting_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(guider_ui.settings_screen_connecting_label, "SCANNING");
        lv_refr_now(NULL);

        if(!wlan_wrpr_connected){
            scan_networks();
        }
    }
    default:
        break;
    }
}

static void settings_screen_connect_to_saved_btn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        lv_obj_clear_flag(guider_ui.settings_screen_connecting_label, LV_OBJ_FLAG_HIDDEN);
        lv_refr_now(NULL);

        char uri_0001[64];

        int ret = wlan_wrpr_connect_saved();
        if(ret == 0)
        {
            softphone_start();
            snprintf(uri_0001, sizeof(uri_0001), "sip:0001@%s", get_local_ip());
            lv_label_set_text(guider_ui.home_screen_ext_num_label, uri_0001);
            lv_obj_set_style_image_recolor(guider_ui.call_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.home_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.incoming_call_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.keypad_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.message_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.settings_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        }
        else{
            lv_label_set_text(guider_ui.settings_screen_connecting_label, "ERROR");
        }

    }
    default:
        break;
    }
}
static void settings_screen_access_point_btn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        lv_obj_clear_flag(guider_ui.settings_screen_connecting_label, LV_OBJ_FLAG_HIDDEN);
        lv_refr_now(NULL);
        char uri_0001[64];

        int ret = wlan_wrpr_start_uap();
        if(ret == 0)
        {
            softphone_start();
            snprintf(uri_0001, sizeof(uri_0001), "sip:0001@%s", get_local_ip());
            lv_label_set_text(guider_ui.home_screen_ext_num_label, uri_0001);
            lv_obj_set_style_image_recolor(guider_ui.call_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.home_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.incoming_call_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.keypad_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor(guider_ui.message_screen_wifi_img, lv_color_hex(0x87CEEB), LV_PART_MAIN | LV_STATE_DEFAULT);
            ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.settings_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        }
        else{
            lv_label_set_text(guider_ui.settings_screen_connecting_label, "ERROR");
        }
    }
    default:
        break;
    }
}

void events_init_settings_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->settings_screen_connect_btn, settings_screen_connect_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->settings_screen_scan_btn, settings_screen_scan_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->settings_screen_connect_to_saved_btn, settings_screen_connect_to_saved_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->settings_screen_access_point_btn, settings_screen_access_point_btn_event_handler, LV_EVENT_ALL, ui);

}


static void message_screen_back_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        incomming_message_flag = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.home_screen, guider_ui.home_screen_del, &guider_ui.message_screen_del, setup_scr_home_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
        break;
    }
    default:
        break;
    }
}

static void message_screen_send_imgbtn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {


        const char * txt = lv_textarea_get_text(guider_ui.message_screen_text_area);



        if(strlen(txt) == 0) return;


        int err = softphone_send_message(sip_uri, txt);

        if(err)
        {
            return;
        }
        display_message(txt, true);

        lv_textarea_set_text(guider_ui.message_screen_text_area, "");

        lv_refr_now(NULL);

        break;
    }
    default:
        break;
    }
}



void events_init_message_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->message_screen_back_imgbtn, message_screen_back_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->message_screen_send_imgbtn, message_screen_send_imgbtn_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}



static void scan_networks(void){

    char combined_ssids[MAX_OUTPUT_LENGTH] = {0};
    WIFI_ScanNetworks();

    while (!WIFI_IsScanDone())
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    wifi_network_t *networks = WIFI_GetNetworks();


    int count = WIFI_GetNetworkCount();

    for (int i = 0; i < count; i++)
    {
        strncat(combined_ssids,networks[i].ssid,WIFI_MAX_SSID_LEN);
        // strncat(combined_ssids, "\n", 1);
        strncat(combined_ssids,"\n",sizeof(combined_ssids) - strlen(combined_ssids) - 1);
    }

    lv_roller_set_options(guider_ui.settings_screen_wifi_roller, combined_ssids, LV_ROLLER_MODE_INFINITE);

    lv_obj_add_flag(guider_ui.settings_screen_connecting_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(guider_ui.settings_screen_connecting_label, "CONNECTING");

    lv_obj_invalidate(guider_ui.settings_screen_wifi_roller);


}

void display_message(const char *msg, bool is_sender)
{
    if(msg == NULL)
    {
        return;
    }

    if(message_count < MAX_CHAT_MESSAGES)
    {
        strcpy(chat_history[message_count].text, msg);
        chat_history[message_count].is_sender = is_sender;

        message_count++;
    }
    else
    {
        for(uint8_t i = 0; i < MAX_CHAT_MESSAGES - 1; i++)
        {
            chat_history[i] = chat_history[i + 1];
        }

        strcpy(chat_history[MAX_CHAT_MESSAGES - 1].text, msg);
        chat_history[MAX_CHAT_MESSAGES - 1].is_sender = is_sender;
    }

    refresh_chat_display();
}
void refresh_chat_display(void)
{
    lv_obj_t *sender_labels[MAX_CHAT_MESSAGES] =
    {
        guider_ui.message_screen_sender_4_label,
        guider_ui.message_screen_sender_3_label,
        guider_ui.message_screen_sender_2_label,
        guider_ui.message_screen_sender_1_label
    };

    lv_obj_t *receiver_labels[MAX_CHAT_MESSAGES] =
    {
        guider_ui.message_screen_receiver_4_label,
        guider_ui.message_screen_receiver_3_label,
        guider_ui.message_screen_receiver_2_label,
        guider_ui.message_screen_receiver_1_label
    };

    uint8_t start_row = MAX_CHAT_MESSAGES - message_count;

    for(uint8_t i = 0; i < MAX_CHAT_MESSAGES; i++)
    {
        lv_obj_add_flag(sender_labels[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(receiver_labels[i], LV_OBJ_FLAG_HIDDEN);
    }

    for(uint8_t msg_idx = 0; msg_idx < message_count; msg_idx++)
    {
        uint8_t row = start_row + msg_idx;

        if(chat_history[msg_idx].is_sender)
        {
            lv_label_set_text(sender_labels[row],
                              chat_history[msg_idx].text);

            lv_obj_clear_flag(sender_labels[row],
                              LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_label_set_text(receiver_labels[row],
                              chat_history[msg_idx].text);

            lv_obj_clear_flag(receiver_labels[row],
                              LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void lvgl_show_home_screen(void *param)
{
    lv_screen_load(guider_ui.home_screen);
}

void lvgl_show_incoming_call(void *param)
{

    incomming_call_flag = true;

    char *peer = (char *)param;

    char ip[64];

    if (softphone_get_peer_ip(ip, sizeof(ip)))
    {

        if (build_uri(peer, ip, sip_prefix, sizeof(sip_prefix), sip_uri, sizeof(sip_uri)))
        {
            PRINTF("Peer IP = %s\r\n", ip);
            lv_label_set_text(guider_ui.incoming_call_screen_incoming_num_label, sip_uri);
            lv_label_set_text(guider_ui.call_screen_num_label, sip_uri);
            lv_label_set_text(guider_ui.home_screen_last_num_label, sip_uri);
        }


    }

    ui_load_scr_animation(&guider_ui, &guider_ui.incoming_call_screen, guider_ui.incoming_call_screen_del, &guider_ui.home_screen_del, setup_scr_incoming_call_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
}

void lvgl_show_incoming_message(void *param){

    char *peer = (char *)param;

    snprintf(sip_uri, sizeof(sip_uri), "%s", peer);

    lv_label_set_text(guider_ui.home_screen_last_num_label, sip_uri);

    
    if(!incomming_message_flag){
        ui_load_scr_animation(&guider_ui, &guider_ui.message_screen, guider_ui.message_screen_del, &guider_ui.home_screen_del, setup_scr_message_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false, false);
    }
    incomming_message_flag = true;

}

const char *get_local_ip(void)
{
    static char ip_str[INET_ADDRSTRLEN];

    /* Walk all registered lwIP netifs and return the first one that is up
     * and has a valid IPv4 address. This works in both STA (terminal) mode
     * and AP (access point) mode, where netif_default may be NULL. */
    struct netif *nif;
    NETIF_FOREACH(nif)
    {
        if (netif_is_up(nif) && !ip4_addr_isany(netif_ip4_addr(nif)))
        {
            ip4addr_ntoa_r(netif_ip4_addr(nif), ip_str, sizeof(ip_str));
            return ip_str;
        }
    }

    strcpy(ip_str, "0.0.0.0");
    return ip_str;
}


bool build_uri(const char *peer, const char *ip, char *sip_prefix, size_t sip_prefix_size, char *uri, size_t uri_size)
{
    const char *at;
    size_t len;

    if (!peer || !ip || !sip_prefix || !uri)
        return false;

    at = strchr(peer, '@');
    if (!at)
        return false;

    len = (at - peer) + 1; /* incluir '@' */


    if (len >= sip_prefix_size)
        len = sip_prefix_size - 1;

    memcpy(sip_prefix, peer, len);
    sip_prefix[len] = '\0';

    snprintf(uri, uri_size, "%s%s", sip_prefix, ip);

    return true;
}