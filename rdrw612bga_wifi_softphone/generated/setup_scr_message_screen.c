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



void setup_scr_message_screen(lv_ui *ui)
{
    //Write codes message_screen
    ui->message_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->message_screen, 480, 320);
    lv_obj_set_scrollbar_mode(ui->message_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for message_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->message_screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_back_imgbtn
    ui->message_screen_back_imgbtn = lv_imagebutton_create(ui->message_screen);
    lv_obj_set_pos(ui->message_screen_back_imgbtn, 33, 262);
    lv_obj_set_size(ui->message_screen_back_imgbtn, 55, 40);
    lv_obj_add_flag(ui->message_screen_back_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->message_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_55x40, NULL, NULL);
    lv_imagebutton_set_src(ui->message_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_back_RGB565A8_55x40, NULL, NULL);
    lv_imagebutton_set_src(ui->message_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_back_RGB565A8_55x40, NULL, NULL);
    lv_imagebutton_set_src(ui->message_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_back_RGB565A8_55x40, NULL, NULL);
    ui->message_screen_back_imgbtn_label = lv_label_create(ui->message_screen_back_imgbtn);
    lv_label_set_text(ui->message_screen_back_imgbtn_label, "");
    lv_label_set_long_mode(ui->message_screen_back_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->message_screen_back_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->message_screen_back_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for message_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->message_screen_back_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_back_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_back_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for message_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->message_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->message_screen_back_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->message_screen_back_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->message_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->message_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for message_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->message_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->message_screen_back_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->message_screen_back_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->message_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->message_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for message_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_back_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->message_screen_back_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes message_screen_cont_1
    ui->message_screen_cont_1 = lv_obj_create(ui->message_screen);
    lv_obj_set_pos(ui->message_screen_cont_1, 145, 0);
    lv_obj_set_size(ui->message_screen_cont_1, 335, 320);
    lv_obj_set_scrollbar_mode(ui->message_screen_cont_1, LV_SCROLLBAR_MODE_ON);

    //Write style for message_screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_sender_1_label
    ui->message_screen_sender_1_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_sender_1_label, 0, 215);
    lv_obj_set_size(ui->message_screen_sender_1_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_sender_1_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_sender_1_label, "Label");
    lv_label_set_long_mode(ui->message_screen_sender_1_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_sender_1_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_sender_1_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_sender_1_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_sender_1_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_sender_1_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_sender_1_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_sender_1_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_sender_1_label, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_sender_1_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_sender_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_receiver_1_label
    ui->message_screen_receiver_1_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_receiver_1_label, 170, 215);
    lv_obj_set_size(ui->message_screen_receiver_1_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_receiver_1_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_receiver_1_label, "Label");
    lv_label_set_long_mode(ui->message_screen_receiver_1_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_receiver_1_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_receiver_1_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_receiver_1_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_receiver_1_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_receiver_1_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_receiver_1_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_receiver_1_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_receiver_1_label, lv_color_hex(0x2FDAAE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_receiver_1_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_receiver_1_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_receiver_2_label
    ui->message_screen_receiver_2_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_receiver_2_label, 170, 165);
    lv_obj_set_size(ui->message_screen_receiver_2_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_receiver_2_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_receiver_2_label, "Label");
    lv_label_set_long_mode(ui->message_screen_receiver_2_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_receiver_2_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_receiver_2_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_receiver_2_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_receiver_2_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_receiver_2_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_receiver_2_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_receiver_2_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_receiver_2_label, lv_color_hex(0x2FDAAE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_receiver_2_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_receiver_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_sender_2_label
    ui->message_screen_sender_2_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_sender_2_label, 0, 165);
    lv_obj_set_size(ui->message_screen_sender_2_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_sender_2_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_sender_2_label, "Label");
    lv_label_set_long_mode(ui->message_screen_sender_2_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_sender_2_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_sender_2_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_sender_2_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_sender_2_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_sender_2_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_sender_2_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_sender_2_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_sender_2_label, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_sender_2_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_sender_2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_receiver_3_label
    ui->message_screen_receiver_3_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_receiver_3_label, 170, 115);
    lv_obj_set_size(ui->message_screen_receiver_3_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_receiver_3_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_receiver_3_label, "Label");
    lv_label_set_long_mode(ui->message_screen_receiver_3_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_receiver_3_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_receiver_3_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_receiver_3_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_receiver_3_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_receiver_3_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_receiver_3_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_receiver_3_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_receiver_3_label, lv_color_hex(0x2FDAAE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_receiver_3_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_receiver_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_sender_3_label
    ui->message_screen_sender_3_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_sender_3_label, 0, 115);
    lv_obj_set_size(ui->message_screen_sender_3_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_sender_3_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_sender_3_label, "Label");
    lv_label_set_long_mode(ui->message_screen_sender_3_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_sender_3_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_sender_3_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_sender_3_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_sender_3_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_sender_3_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_sender_3_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_sender_3_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_sender_3_label, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_sender_3_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_sender_3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_receiver_4_label
    ui->message_screen_receiver_4_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_receiver_4_label, 170, 65);
    lv_obj_set_size(ui->message_screen_receiver_4_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_receiver_4_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_receiver_4_label, "Label");
    lv_label_set_long_mode(ui->message_screen_receiver_4_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_receiver_4_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_receiver_4_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_receiver_4_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_receiver_4_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_receiver_4_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_receiver_4_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_receiver_4_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_receiver_4_label, lv_color_hex(0x2FDAAE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_receiver_4_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_receiver_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_sender_4_label
    ui->message_screen_sender_4_label = lv_label_create(ui->message_screen_cont_1);
    lv_obj_set_pos(ui->message_screen_sender_4_label, 0, 65);
    lv_obj_set_size(ui->message_screen_sender_4_label, 165, 32);
    lv_obj_add_flag(ui->message_screen_sender_4_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->message_screen_sender_4_label, "Label");
    lv_label_set_long_mode(ui->message_screen_sender_4_label, LV_LABEL_LONG_WRAP);

    //Write style for message_screen_sender_4_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_sender_4_label, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->message_screen_sender_4_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_sender_4_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_sender_4_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_sender_4_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_sender_4_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_sender_4_label, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_sender_4_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_sender_4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes message_screen_text_area
    ui->message_screen_text_area = lv_textarea_create(ui->message_screen);
    lv_obj_set_pos(ui->message_screen_text_area, 149, 250);
    lv_obj_set_size(ui->message_screen_text_area, 255, 64);
    lv_textarea_set_text(ui->message_screen_text_area, "");
    lv_textarea_set_placeholder_text(ui->message_screen_text_area, "");
    lv_textarea_set_password_bullet(ui->message_screen_text_area, "*");
    lv_textarea_set_password_mode(ui->message_screen_text_area, false);
    lv_textarea_set_one_line(ui->message_screen_text_area, false);
    lv_textarea_set_accepted_chars(ui->message_screen_text_area, "");
    lv_textarea_set_max_length(ui->message_screen_text_area, 32);
#if LV_USE_KEYBOARD
    lv_obj_add_event_cb(ui->message_screen_text_area, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif

    //Write style for message_screen_text_area, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->message_screen_text_area, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_text_area, &lv_font_Antonio_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_text_area, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->message_screen_text_area, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_text_area, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->message_screen_text_area, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_text_area, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_text_area, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->message_screen_text_area, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->message_screen_text_area, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->message_screen_text_area, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->message_screen_text_area, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_text_area, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->message_screen_text_area, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->message_screen_text_area, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->message_screen_text_area, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_text_area, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for message_screen_text_area, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->message_screen_text_area, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->message_screen_text_area, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->message_screen_text_area, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->message_screen_text_area, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes message_screen_send_imgbtn
    ui->message_screen_send_imgbtn = lv_imagebutton_create(ui->message_screen);
    lv_obj_set_pos(ui->message_screen_send_imgbtn, 410, 254);
    lv_obj_set_size(ui->message_screen_send_imgbtn, 65, 55);
    lv_obj_add_flag(ui->message_screen_send_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->message_screen_send_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_send_RGB565A8_65x55, NULL, NULL);
    lv_imagebutton_set_src(ui->message_screen_send_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_send_RGB565A8_65x55, NULL, NULL);
    lv_imagebutton_set_src(ui->message_screen_send_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_send_RGB565A8_65x55, NULL, NULL);
    lv_imagebutton_set_src(ui->message_screen_send_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_send_RGB565A8_65x55, NULL, NULL);
    ui->message_screen_send_imgbtn_label = lv_label_create(ui->message_screen_send_imgbtn);
    lv_label_set_text(ui->message_screen_send_imgbtn_label, "");
    lv_label_set_long_mode(ui->message_screen_send_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->message_screen_send_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->message_screen_send_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for message_screen_send_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->message_screen_send_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->message_screen_send_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->message_screen_send_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->message_screen_send_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for message_screen_send_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_recolor(ui->message_screen_send_imgbtn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->message_screen_send_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->message_screen_send_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->message_screen_send_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for message_screen_send_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_recolor(ui->message_screen_send_imgbtn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->message_screen_send_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->message_screen_send_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->message_screen_send_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for message_screen_send_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_recolor(ui->message_screen_send_imgbtn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->message_screen_send_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes message_screen_line_1
    ui->message_screen_line_1 = lv_line_create(ui->message_screen);
    lv_obj_set_pos(ui->message_screen_line_1, 132, 16);
    lv_obj_set_size(ui->message_screen_line_1, 5, 299);
    static lv_point_precise_t message_screen_line_1[] = {{0, 0},{0, 290}};
    lv_line_set_points(ui->message_screen_line_1, message_screen_line_1, 2);

    //Write style for message_screen_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->message_screen_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->message_screen_line_1, lv_color_hex(0x757575), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->message_screen_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->message_screen_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);


    //Write codes message_screen_wifi_img
    ui->message_screen_wifi_img = lv_image_create(ui->message_screen);
    lv_obj_set_pos(ui->message_screen_wifi_img, 345, 18);
    lv_obj_set_size(ui->message_screen_wifi_img, 38, 38);
    lv_obj_add_flag(ui->message_screen_wifi_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->message_screen_wifi_img, &_wifi_RGB565A8_38x38);
    lv_image_set_pivot(ui->message_screen_wifi_img, 50,50);
    lv_image_set_rotation(ui->message_screen_wifi_img, 0);

    //Write style for message_screen_wifi_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->message_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->message_screen_wifi_img, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->message_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of message_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->message_screen);

    //Init events for screen.
    events_init_message_screen(ui);
}
