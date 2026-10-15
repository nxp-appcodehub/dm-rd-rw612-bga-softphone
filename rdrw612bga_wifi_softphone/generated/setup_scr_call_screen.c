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



void setup_scr_call_screen(lv_ui *ui)
{
    //Write codes call_screen
    ui->call_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->call_screen, 480, 320);
    lv_obj_set_scrollbar_mode(ui->call_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for call_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->call_screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->call_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->call_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes call_screen_endcall_imgbtn
    ui->call_screen_endcall_imgbtn = lv_imagebutton_create(ui->call_screen);
    lv_obj_set_pos(ui->call_screen_endcall_imgbtn, 206, 199);
    lv_obj_set_size(ui->call_screen_endcall_imgbtn, 85, 85);
    lv_obj_add_flag(ui->call_screen_endcall_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->call_screen_endcall_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_call_end_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->call_screen_endcall_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_call_end_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->call_screen_endcall_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_call_end_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->call_screen_endcall_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_call_end_RGB565A8_85x85, NULL, NULL);
    ui->call_screen_endcall_imgbtn_label = lv_label_create(ui->call_screen_endcall_imgbtn);
    lv_label_set_text(ui->call_screen_endcall_imgbtn_label, "");
    lv_label_set_long_mode(ui->call_screen_endcall_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->call_screen_endcall_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->call_screen_endcall_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for call_screen_endcall_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->call_screen_endcall_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->call_screen_endcall_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->call_screen_endcall_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->call_screen_endcall_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->call_screen_endcall_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for call_screen_endcall_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->call_screen_endcall_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->call_screen_endcall_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->call_screen_endcall_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->call_screen_endcall_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->call_screen_endcall_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->call_screen_endcall_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for call_screen_endcall_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->call_screen_endcall_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->call_screen_endcall_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->call_screen_endcall_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->call_screen_endcall_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->call_screen_endcall_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->call_screen_endcall_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for call_screen_endcall_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->call_screen_endcall_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->call_screen_endcall_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes call_screen_num_label
    ui->call_screen_num_label = lv_label_create(ui->call_screen);
    lv_obj_set_pos(ui->call_screen_num_label, 125, 132);
    lv_obj_set_size(ui->call_screen_num_label, 245, 32);
    lv_label_set_text(ui->call_screen_num_label, "####");
    lv_label_set_long_mode(ui->call_screen_num_label, LV_LABEL_LONG_WRAP);

    //Write style for call_screen_num_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->call_screen_num_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->call_screen_num_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->call_screen_num_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->call_screen_num_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->call_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    //Write codes call_screen_wifi_img
    ui->call_screen_wifi_img = lv_image_create(ui->call_screen);
    lv_obj_set_pos(ui->call_screen_wifi_img, 345, 18);
    lv_obj_set_size(ui->call_screen_wifi_img, 38, 38);
    lv_obj_add_flag(ui->call_screen_wifi_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->call_screen_wifi_img, &_wifi_RGB565A8_38x38);
    lv_image_set_pivot(ui->call_screen_wifi_img, 50,50);
    lv_image_set_rotation(ui->call_screen_wifi_img, 0);

    //Write style for call_screen_wifi_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->call_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->call_screen_wifi_img, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->call_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of call_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->call_screen);

    //Init events for screen.
    events_init_call_screen(ui);
}
