/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

#include "wlan_wrpr.h"



void setup_scr_settings_screen(lv_ui *ui)
{
    //Write codes settings_screen
    ui->settings_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->settings_screen, 480, 320);
    lv_obj_set_scrollbar_mode(ui->settings_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for settings_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_select_label
    ui->settings_screen_select_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_select_label, 26, 28);
    lv_obj_set_size(ui->settings_screen_select_label, 179, 32);
    lv_label_set_text(ui->settings_screen_select_label, "SEELCT YOUR WI-FI NETWORK:");
    lv_label_set_long_mode(ui->settings_screen_select_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_select_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_select_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_select_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_select_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_select_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_select_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_ssid_label
    ui->settings_screen_ssid_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_ssid_label, 14, 67);
    lv_obj_set_size(ui->settings_screen_ssid_label, 74, 32);
    lv_label_set_text(ui->settings_screen_ssid_label, "SSID:");
    lv_label_set_long_mode(ui->settings_screen_ssid_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_ssid_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_ssid_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_ssid_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_ssid_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_ssid_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_password_label
    ui->settings_screen_password_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_password_label, 233, 67);
    lv_obj_set_size(ui->settings_screen_password_label, 100, 32);
    lv_label_set_text(ui->settings_screen_password_label, "PASSWORD:");
    lv_label_set_long_mode(ui->settings_screen_password_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_password_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_password_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_password_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_password_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_password_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_password_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_password_keyboard
    ui->settings_screen_password_keyboard = lv_textarea_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_password_keyboard, 247, 88);
    lv_obj_set_size(ui->settings_screen_password_keyboard, 188, 27);
    lv_textarea_set_text(ui->settings_screen_password_keyboard, "");
    lv_textarea_set_placeholder_text(ui->settings_screen_password_keyboard, "PASSWORD");
    lv_textarea_set_password_bullet(ui->settings_screen_password_keyboard, "*");
    lv_textarea_set_password_mode(ui->settings_screen_password_keyboard, false);
    lv_textarea_set_one_line(ui->settings_screen_password_keyboard, true);
    lv_textarea_set_accepted_chars(ui->settings_screen_password_keyboard, "");
    lv_textarea_set_max_length(ui->settings_screen_password_keyboard, 32);
#if LV_USE_KEYBOARD
    lv_obj_add_event_cb(ui->settings_screen_password_keyboard, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif

    //Write style for settings_screen_password_keyboard, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->settings_screen_password_keyboard, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_password_keyboard, &lv_font_Antonio_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_password_keyboard, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_password_keyboard, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_password_keyboard, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_password_keyboard, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_password_keyboard, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_password_keyboard, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->settings_screen_password_keyboard, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->settings_screen_password_keyboard, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->settings_screen_password_keyboard, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->settings_screen_password_keyboard, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_password_keyboard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_password_keyboard, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_password_keyboard, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_password_keyboard, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_password_keyboard, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for settings_screen_password_keyboard, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen_password_keyboard, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_password_keyboard, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_password_keyboard, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_password_keyboard, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes settings_screen_connect_btn
    ui->settings_screen_connect_btn = lv_button_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_connect_btn, 108, 271);
    lv_obj_set_size(ui->settings_screen_connect_btn, 110, 45);
    ui->settings_screen_connect_btn_label = lv_label_create(ui->settings_screen_connect_btn);
    lv_label_set_text(ui->settings_screen_connect_btn_label, "CONNECT");
    lv_label_set_long_mode(ui->settings_screen_connect_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->settings_screen_connect_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->settings_screen_connect_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->settings_screen_connect_btn_label, LV_PCT(100));

    //Write style for settings_screen_connect_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen_connect_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_connect_btn, lv_color_hex(0x54b0e7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_connect_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->settings_screen_connect_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_connect_btn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_connect_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_connect_btn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_connect_btn, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_connect_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_connect_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_connecting_label
    ui->settings_screen_connecting_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_connecting_label, 150, 192);
    lv_obj_set_size(ui->settings_screen_connecting_label, 179, 32);
    lv_obj_add_flag(ui->settings_screen_connecting_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->settings_screen_connecting_label, "CONNECTING");
    lv_label_set_long_mode(ui->settings_screen_connecting_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_connecting_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_connecting_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_connecting_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_connecting_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_connecting_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_connecting_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_wifi_roller
    ui->settings_screen_wifi_roller = lv_roller_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_wifi_roller, 33, 88);
    lv_obj_set_width(ui->settings_screen_wifi_roller, 188);
    lv_roller_set_options(ui->settings_screen_wifi_roller, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10", LV_ROLLER_MODE_INFINITE);

    //Write style for settings_screen_wifi_roller, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->settings_screen_wifi_roller, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_wifi_roller, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_wifi_roller, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_wifi_roller, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_wifi_roller, lv_color_hex(0x333333), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_wifi_roller, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_wifi_roller, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_wifi_roller, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->settings_screen_wifi_roller, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->settings_screen_wifi_roller, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->settings_screen_wifi_roller, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->settings_screen_wifi_roller, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_wifi_roller, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_wifi_roller, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_wifi_roller, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for settings_screen_wifi_roller, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen_wifi_roller, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_wifi_roller, lv_color_hex(0x2195f6), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_wifi_roller, LV_GRAD_DIR_NONE, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_wifi_roller, lv_color_hex(0xFFFFFF), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_wifi_roller, &lv_font_montserratMedium_12, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_wifi_roller, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->settings_screen_wifi_roller, 3);
     //Write codes settings_screen_scan_btn
    ui->settings_screen_scan_btn = lv_button_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_scan_btn, 108, 210);
    lv_obj_set_size(ui->settings_screen_scan_btn, 110, 45);
    ui->settings_screen_scan_btn_label = lv_label_create(ui->settings_screen_scan_btn);
    lv_label_set_text(ui->settings_screen_scan_btn_label, "SCAN");
    lv_label_set_long_mode(ui->settings_screen_scan_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->settings_screen_scan_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->settings_screen_scan_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->settings_screen_scan_btn_label, LV_PCT(100));

    //Write style for settings_screen_scan_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen_scan_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_scan_btn, lv_color_hex(0x54b0e7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_scan_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->settings_screen_scan_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_scan_btn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_scan_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_scan_btn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_scan_btn, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_scan_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_scan_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_connect_to_saved_btn
    ui->settings_screen_connect_to_saved_btn = lv_button_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_connect_to_saved_btn, 263, 210);
    lv_obj_set_size(ui->settings_screen_connect_to_saved_btn, 110, 45);
    ui->settings_screen_connect_to_saved_btn_label = lv_label_create(ui->settings_screen_connect_to_saved_btn);
    lv_label_set_text(ui->settings_screen_connect_to_saved_btn_label, "CONNECT TO SAVED");
    lv_label_set_long_mode(ui->settings_screen_connect_to_saved_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->settings_screen_connect_to_saved_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->settings_screen_connect_to_saved_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->settings_screen_connect_to_saved_btn_label, LV_PCT(100));

    //Write style for settings_screen_connect_to_saved_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen_connect_to_saved_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_connect_to_saved_btn, lv_color_hex(0x54b0e7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_connect_to_saved_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->settings_screen_connect_to_saved_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_connect_to_saved_btn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_connect_to_saved_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_connect_to_saved_btn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_connect_to_saved_btn, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_connect_to_saved_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_connect_to_saved_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_access_point_btn
    ui->settings_screen_access_point_btn = lv_button_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_access_point_btn, 263, 271);
    lv_obj_set_size(ui->settings_screen_access_point_btn, 110, 45);
    ui->settings_screen_access_point_btn_label = lv_label_create(ui->settings_screen_access_point_btn);
    lv_label_set_text(ui->settings_screen_access_point_btn_label, "ACCESS POINT");
    lv_label_set_long_mode(ui->settings_screen_access_point_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->settings_screen_access_point_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->settings_screen_access_point_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->settings_screen_access_point_btn_label, LV_PCT(100));

    //Write style for settings_screen_access_point_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->settings_screen_access_point_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->settings_screen_access_point_btn, lv_color_hex(0x54b0e7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->settings_screen_access_point_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->settings_screen_access_point_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_access_point_btn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_access_point_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_access_point_btn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_access_point_btn, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_access_point_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_access_point_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_saved_net_label
    ui->settings_screen_saved_net_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_saved_net_label, 231, 130);
    lv_obj_set_size(ui->settings_screen_saved_net_label, 97, 28);
    lv_label_set_text(ui->settings_screen_saved_net_label, "SAVED NET:");
    lv_label_set_long_mode(ui->settings_screen_saved_net_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_saved_net_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_saved_net_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_saved_net_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_saved_net_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_saved_net_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_saved_net_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_saved_net_ssid_label
    ui->settings_screen_saved_net_ssid_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_saved_net_ssid_label, 318, 130);
    lv_obj_set_size(ui->settings_screen_saved_net_ssid_label, 161, 17);
    lv_label_set_text(ui->settings_screen_saved_net_ssid_label, wlan_wrpr_ssid);
    lv_label_set_long_mode(ui->settings_screen_saved_net_ssid_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_saved_net_ssid_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_saved_net_ssid_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_saved_net_ssid_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_saved_net_ssid_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_saved_net_ssid_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_saved_net_ssid_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_saved_pass_label
    ui->settings_screen_saved_pass_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_saved_pass_label, 233, 158);
    lv_obj_set_size(ui->settings_screen_saved_pass_label, 97, 28);
    lv_label_set_text(ui->settings_screen_saved_pass_label, "PASSWORD:");
    lv_label_set_long_mode(ui->settings_screen_saved_pass_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_saved_pass_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_saved_pass_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_saved_pass_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_saved_pass_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_saved_pass_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_saved_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes settings_screen_saved_net_pass_label
    ui->settings_screen_saved_net_pass_label = lv_label_create(ui->settings_screen);
    lv_obj_set_pos(ui->settings_screen_saved_net_pass_label, 318, 158);
    lv_obj_set_size(ui->settings_screen_saved_net_pass_label, 161, 17);
    lv_label_set_text(ui->settings_screen_saved_net_pass_label, wlan_wrpr_passphrase);
    lv_label_set_long_mode(ui->settings_screen_saved_net_pass_label, LV_LABEL_LONG_WRAP);

    //Write style for settings_screen_saved_net_pass_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->settings_screen_saved_net_pass_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->settings_screen_saved_net_pass_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->settings_screen_saved_net_pass_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->settings_screen_saved_net_pass_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->settings_screen_saved_net_pass_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    //The custom code of settings_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->settings_screen);

    //Init events for screen.
    events_init_settings_screen(ui);
}
