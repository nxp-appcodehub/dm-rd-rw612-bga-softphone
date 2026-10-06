/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *home_screen;
	bool home_screen_del;
	lv_obj_t *home_screen_wifi_img;
	lv_obj_t *home_screen_ext_label;
	lv_obj_t *home_screen_ext_num_label;
	lv_obj_t *home_screen_last_num_label;
	lv_obj_t *home_screen_last_label;
	lv_obj_t *home_screen_call_imgbtn;
	lv_obj_t *home_screen_call_imgbtn_label;
	lv_obj_t *home_screen_keypad_imgbtn;
	lv_obj_t *home_screen_keypad_imgbtn_label;
	lv_obj_t *home_screen_chat_imgbtn;
	lv_obj_t *home_screen_chat_imgbtn_label;
	lv_obj_t *incoming_call_screen;
	bool incoming_call_screen_del;
	lv_obj_t *incoming_call_screen_reject_imgbtn;
	lv_obj_t *incoming_call_screen_reject_imgbtn_label;
	lv_obj_t *incoming_call_screen_answer_imgbtn;
	lv_obj_t *incoming_call_screen_answer_imgbtn_label;
	lv_obj_t *incoming_call_screen_wifi_img;
	lv_obj_t *incoming_call_screen_incoming_label;
	lv_obj_t *incoming_call_screen_incoming_num_label;
	lv_obj_t *keypad_screen;
	bool keypad_screen_del;
	lv_obj_t *keypad_screen_wifi_img;
	lv_obj_t *keypad_screen_keypad;
	lv_obj_t *keypad_screen_call_imgbtn;
	lv_obj_t *keypad_screen_call_imgbtn_label;
	lv_obj_t *keypad_screen_num_label;
	lv_obj_t *keypad_screen_back_imgbtn;
	lv_obj_t *keypad_screen_back_imgbtn_label;
	lv_obj_t *call_screen;
	bool call_screen_del;
	lv_obj_t *call_screen_endcall_imgbtn;
	lv_obj_t *call_screen_endcall_imgbtn_label;
	lv_obj_t *call_screen_num_label;
	lv_obj_t *call_screen_wifi_img;
	lv_obj_t *settings_screen;
	bool settings_screen_del;
	lv_obj_t *settings_screen_select_label;
	lv_obj_t *settings_screen_ssid_label;
	lv_obj_t *settings_screen_password_label;
	lv_obj_t *settings_screen_password_keyboard;
	lv_obj_t *settings_screen_connect_btn;
	lv_obj_t *settings_screen_connect_btn_label;
	lv_obj_t *settings_screen_connecting_label;
	lv_obj_t *settings_screen_wifi_roller;
	lv_obj_t *settings_screen_scan_btn;
	lv_obj_t *settings_screen_scan_btn_label;
	lv_obj_t *settings_screen_connect_to_saved_btn;
	lv_obj_t *settings_screen_connect_to_saved_btn_label;
	lv_obj_t *settings_screen_access_point_btn;
	lv_obj_t *settings_screen_access_point_btn_label;
	lv_obj_t *settings_screen_saved_net_label;
	lv_obj_t *settings_screen_saved_net_ssid_label;
	lv_obj_t *settings_screen_saved_pass_label;
	lv_obj_t *settings_screen_saved_net_pass_label;
	lv_obj_t *message_screen;
	bool message_screen_del;
	lv_obj_t *message_screen_back_imgbtn;
	lv_obj_t *message_screen_back_imgbtn_label;
	lv_obj_t *message_screen_cont_1;
	lv_obj_t *message_screen_sender_1_label;
	lv_obj_t *message_screen_receiver_1_label;
	lv_obj_t *message_screen_receiver_2_label;
	lv_obj_t *message_screen_sender_2_label;
	lv_obj_t *message_screen_receiver_3_label;
	lv_obj_t *message_screen_sender_3_label;
	lv_obj_t *message_screen_receiver_4_label;
	lv_obj_t *message_screen_sender_4_label;
	lv_obj_t *message_screen_text_area;
	lv_obj_t *message_screen_send_imgbtn;
	lv_obj_t *message_screen_send_imgbtn_label;
	lv_obj_t *message_screen_line_1;
	lv_obj_t *message_screen_wifi_img;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_home_screen(lv_ui *ui);
void setup_scr_incoming_call_screen(lv_ui *ui);
void setup_scr_keypad_screen(lv_ui *ui);
void setup_scr_call_screen(lv_ui *ui);
void setup_scr_settings_screen(lv_ui *ui);
void setup_scr_message_screen(lv_ui *ui);
LV_IMAGE_DECLARE(_wifi_RGB565A8_38x38);
LV_IMAGE_DECLARE(_call_RGB565A8_97x87);
LV_IMAGE_DECLARE(_settings_RGB565A8_60x40);
LV_IMAGE_DECLARE(_keypad_RGB565A8_115x97);
LV_IMAGE_DECLARE(_chat_RGB565A8_65x55);
LV_IMAGE_DECLARE(_call_end_RGB565A8_85x85);
LV_IMAGE_DECLARE(_call_RGB565A8_85x85);
LV_IMAGE_DECLARE(_back_RGB565A8_55x40);
LV_IMAGE_DECLARE(_back_RGB565A8_35x40);
LV_IMAGE_DECLARE(_send_RGB565A8_65x55);

LV_FONT_DECLARE(lv_font_Antonio_Regular_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_Antonio_Regular_12)
LV_FONT_DECLARE(lv_font_FontAwesome5_12)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_18)
LV_FONT_DECLARE(lv_font_Antonio_12)


#ifdef __cplusplus
}
#endif
#endif
