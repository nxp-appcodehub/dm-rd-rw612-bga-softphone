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



void setup_scr_home_screen(lv_ui *ui)
{
    //Write codes home_screen
    ui->home_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->home_screen, 480, 320);
    lv_obj_set_scrollbar_mode(ui->home_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for home_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_screen_wifi_img
    ui->home_screen_wifi_img = lv_image_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_wifi_img, 345, 18);
    lv_obj_set_size(ui->home_screen_wifi_img, 38, 38);
    lv_obj_add_flag(ui->home_screen_wifi_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->home_screen_wifi_img, &_wifi_RGB565A8_38x38);
    lv_image_set_pivot(ui->home_screen_wifi_img, 50,50);
    lv_image_set_rotation(ui->home_screen_wifi_img, 0);

    //Write style for home_screen_wifi_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->home_screen_wifi_img, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->home_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_screen_ext_label
    ui->home_screen_ext_label = lv_label_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_ext_label, 29, 17);
    lv_obj_set_size(ui->home_screen_ext_label, 62, 21);
    lv_label_set_text(ui->home_screen_ext_label, "EXT:");
    lv_label_set_long_mode(ui->home_screen_ext_label, LV_LABEL_LONG_WRAP);

    //Write style for home_screen_ext_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_screen_ext_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_ext_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_ext_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_ext_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_ext_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_screen_ext_num_label
    ui->home_screen_ext_num_label = lv_label_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_ext_num_label, 98, 17);
    lv_obj_set_size(ui->home_screen_ext_num_label, 245, 21);
    lv_label_set_text(ui->home_screen_ext_num_label, "####");
    lv_label_set_long_mode(ui->home_screen_ext_num_label, LV_LABEL_LONG_WRAP);

    //Write style for home_screen_ext_num_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_screen_ext_num_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_ext_num_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_ext_num_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_ext_num_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_ext_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_screen_last_num_label
    ui->home_screen_last_num_label = lv_label_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_last_num_label, 98, 50);
    lv_obj_set_size(ui->home_screen_last_num_label, 245, 21);
    lv_label_set_text(ui->home_screen_last_num_label, "####");
    lv_label_set_long_mode(ui->home_screen_last_num_label, LV_LABEL_LONG_WRAP);

    //Write style for home_screen_last_num_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_screen_last_num_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_last_num_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_last_num_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_last_num_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_last_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_screen_last_label
    ui->home_screen_last_label = lv_label_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_last_label, 29, 50);
    lv_obj_set_size(ui->home_screen_last_label, 62, 21);
    lv_label_set_text(ui->home_screen_last_label, "LAST:");
    lv_label_set_long_mode(ui->home_screen_last_label, LV_LABEL_LONG_WRAP);

    //Write style for home_screen_last_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_screen_last_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_last_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_last_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_last_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_last_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_screen_call_imgbtn
    ui->home_screen_call_imgbtn = lv_imagebutton_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_call_imgbtn, 293, 145);
    lv_obj_set_size(ui->home_screen_call_imgbtn, 97, 87);
    lv_obj_add_flag(ui->home_screen_call_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->home_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_call_RGB565A8_97x87, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_call_RGB565A8_97x87, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_call_RGB565A8_97x87, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_call_RGB565A8_97x87, NULL, NULL);
    ui->home_screen_call_imgbtn_label = lv_label_create(ui->home_screen_call_imgbtn);
    lv_label_set_text(ui->home_screen_call_imgbtn_label, "");
    lv_label_set_long_mode(ui->home_screen_call_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_screen_call_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_screen_call_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for home_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->home_screen_call_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_call_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_call_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for home_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->home_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->home_screen_call_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->home_screen_call_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->home_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->home_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for home_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->home_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->home_screen_call_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->home_screen_call_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->home_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->home_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for home_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_call_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->home_screen_call_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);


    //Write codes home_screen_keypad_imgbtn
    ui->home_screen_keypad_imgbtn = lv_imagebutton_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_keypad_imgbtn, 101, 137);
    lv_obj_set_size(ui->home_screen_keypad_imgbtn, 115, 97);
    lv_obj_add_flag(ui->home_screen_keypad_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->home_screen_keypad_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_keypad_RGB565A8_115x97, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_keypad_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_keypad_RGB565A8_115x97, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_keypad_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_keypad_RGB565A8_115x97, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_keypad_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_keypad_RGB565A8_115x97, NULL, NULL);
    ui->home_screen_keypad_imgbtn_label = lv_label_create(ui->home_screen_keypad_imgbtn);
    lv_label_set_text(ui->home_screen_keypad_imgbtn_label, "");
    lv_label_set_long_mode(ui->home_screen_keypad_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_screen_keypad_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_screen_keypad_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for home_screen_keypad_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->home_screen_keypad_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_keypad_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_keypad_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_keypad_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_keypad_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for home_screen_keypad_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_keypad_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->home_screen_keypad_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->home_screen_keypad_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->home_screen_keypad_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->home_screen_keypad_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->home_screen_keypad_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for home_screen_keypad_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_keypad_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->home_screen_keypad_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->home_screen_keypad_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->home_screen_keypad_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->home_screen_keypad_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->home_screen_keypad_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for home_screen_keypad_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_keypad_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->home_screen_keypad_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

     //Write codes home_screen_chat_imgbtn
    ui->home_screen_chat_imgbtn = lv_imagebutton_create(ui->home_screen);
    lv_obj_set_pos(ui->home_screen_chat_imgbtn, 410, 254);
    lv_obj_set_size(ui->home_screen_chat_imgbtn, 65, 55);
    lv_obj_add_flag(ui->home_screen_chat_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->home_screen_chat_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_chat_RGB565A8_65x55, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_chat_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_chat_RGB565A8_65x55, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_chat_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_chat_RGB565A8_65x55, NULL, NULL);
    lv_imagebutton_set_src(ui->home_screen_chat_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_chat_RGB565A8_65x55, NULL, NULL);
    ui->home_screen_chat_imgbtn_label = lv_label_create(ui->home_screen_chat_imgbtn);
    lv_label_set_text(ui->home_screen_chat_imgbtn_label, "");
    lv_label_set_long_mode(ui->home_screen_chat_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_screen_chat_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_screen_chat_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for home_screen_chat_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->home_screen_chat_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_screen_chat_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_screen_chat_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_screen_chat_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_screen_chat_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for home_screen_chat_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_chat_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->home_screen_chat_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->home_screen_chat_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->home_screen_chat_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->home_screen_chat_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->home_screen_chat_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for home_screen_chat_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_chat_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->home_screen_chat_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->home_screen_chat_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->home_screen_chat_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->home_screen_chat_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->home_screen_chat_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for home_screen_chat_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->home_screen_chat_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->home_screen_chat_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);


    //The custom code of home_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->home_screen);

    //Init events for screen.
    events_init_home_screen(ui);
}
