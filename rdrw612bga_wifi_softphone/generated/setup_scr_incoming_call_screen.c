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



void setup_scr_incoming_call_screen(lv_ui *ui)
{
    //Write codes incoming_call_screen
    ui->incoming_call_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->incoming_call_screen, 480, 320);
    lv_obj_set_scrollbar_mode(ui->incoming_call_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for incoming_call_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->incoming_call_screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->incoming_call_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->incoming_call_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes incoming_call_screen_reject_imgbtn
    ui->incoming_call_screen_reject_imgbtn = lv_imagebutton_create(ui->incoming_call_screen);
    lv_obj_set_pos(ui->incoming_call_screen_reject_imgbtn, 139, 189);
    lv_obj_set_size(ui->incoming_call_screen_reject_imgbtn, 85, 85);
    lv_obj_add_flag(ui->incoming_call_screen_reject_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->incoming_call_screen_reject_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_call_end_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->incoming_call_screen_reject_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_call_end_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->incoming_call_screen_reject_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_call_end_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->incoming_call_screen_reject_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_call_end_RGB565A8_85x85, NULL, NULL);
    ui->incoming_call_screen_reject_imgbtn_label = lv_label_create(ui->incoming_call_screen_reject_imgbtn);
    lv_label_set_text(ui->incoming_call_screen_reject_imgbtn_label, "");
    lv_label_set_long_mode(ui->incoming_call_screen_reject_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->incoming_call_screen_reject_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->incoming_call_screen_reject_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for incoming_call_screen_reject_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->incoming_call_screen_reject_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->incoming_call_screen_reject_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_reject_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->incoming_call_screen_reject_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_reject_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for incoming_call_screen_reject_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_reject_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_reject_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->incoming_call_screen_reject_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->incoming_call_screen_reject_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_reject_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_reject_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for incoming_call_screen_reject_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_reject_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_reject_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->incoming_call_screen_reject_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->incoming_call_screen_reject_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_reject_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_reject_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for incoming_call_screen_reject_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_reject_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_reject_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes incoming_call_screen_answer_imgbtn
    ui->incoming_call_screen_answer_imgbtn = lv_imagebutton_create(ui->incoming_call_screen);
    lv_obj_set_pos(ui->incoming_call_screen_answer_imgbtn, 260, 189);
    lv_obj_set_size(ui->incoming_call_screen_answer_imgbtn, 85, 85);
    lv_obj_add_flag(ui->incoming_call_screen_answer_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->incoming_call_screen_answer_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_call_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->incoming_call_screen_answer_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_call_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->incoming_call_screen_answer_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_call_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->incoming_call_screen_answer_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_call_RGB565A8_85x85, NULL, NULL);
    ui->incoming_call_screen_answer_imgbtn_label = lv_label_create(ui->incoming_call_screen_answer_imgbtn);
    lv_label_set_text(ui->incoming_call_screen_answer_imgbtn_label, "");
    lv_label_set_long_mode(ui->incoming_call_screen_answer_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->incoming_call_screen_answer_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->incoming_call_screen_answer_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for incoming_call_screen_answer_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->incoming_call_screen_answer_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->incoming_call_screen_answer_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_answer_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->incoming_call_screen_answer_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_answer_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for incoming_call_screen_answer_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_answer_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_answer_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->incoming_call_screen_answer_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->incoming_call_screen_answer_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_answer_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_answer_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for incoming_call_screen_answer_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_answer_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_answer_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->incoming_call_screen_answer_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->incoming_call_screen_answer_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_answer_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_answer_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for incoming_call_screen_answer_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_answer_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_answer_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);


    //Write codes incoming_call_screen_wifi_img
    ui->incoming_call_screen_wifi_img = lv_image_create(ui->incoming_call_screen);
    lv_obj_set_pos(ui->incoming_call_screen_wifi_img, 345, 18);
    lv_obj_set_size(ui->incoming_call_screen_wifi_img, 38, 38);
    lv_obj_add_flag(ui->incoming_call_screen_wifi_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->incoming_call_screen_wifi_img, &_wifi_RGB565A8_38x38);
    lv_image_set_pivot(ui->incoming_call_screen_wifi_img, 50,50);
    lv_image_set_rotation(ui->incoming_call_screen_wifi_img, 0);

    //Write style for incoming_call_screen_wifi_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->incoming_call_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->incoming_call_screen_wifi_img, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->incoming_call_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes incoming_call_screen_incoming_label
    ui->incoming_call_screen_incoming_label = lv_label_create(ui->incoming_call_screen);
    lv_obj_set_pos(ui->incoming_call_screen_incoming_label, 156, 109);
    lv_obj_set_size(ui->incoming_call_screen_incoming_label, 163, 32);
    lv_label_set_text(ui->incoming_call_screen_incoming_label, "INCOMING CALL FROM:");
    lv_label_set_long_mode(ui->incoming_call_screen_incoming_label, LV_LABEL_LONG_WRAP);

    //Write style for incoming_call_screen_incoming_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->incoming_call_screen_incoming_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->incoming_call_screen_incoming_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_incoming_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->incoming_call_screen_incoming_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_incoming_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes incoming_call_screen_incoming_num_label
    ui->incoming_call_screen_incoming_num_label = lv_label_create(ui->incoming_call_screen);
    lv_obj_set_pos(ui->incoming_call_screen_incoming_num_label, 125, 141);
    lv_obj_set_size(ui->incoming_call_screen_incoming_num_label, 245, 21);
    lv_label_set_text(ui->incoming_call_screen_incoming_num_label, "####");
    lv_label_set_long_mode(ui->incoming_call_screen_incoming_num_label, LV_LABEL_LONG_WRAP);

    //Write style for incoming_call_screen_incoming_num_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->incoming_call_screen_incoming_num_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->incoming_call_screen_incoming_num_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->incoming_call_screen_incoming_num_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->incoming_call_screen_incoming_num_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->incoming_call_screen_incoming_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of incoming_call_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->incoming_call_screen);

    //Init events for screen.
    events_init_incoming_call_screen(ui);
}
