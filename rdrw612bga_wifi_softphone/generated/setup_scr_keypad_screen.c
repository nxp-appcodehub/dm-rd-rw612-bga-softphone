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



void setup_scr_keypad_screen(lv_ui *ui)
{
    //Write codes keypad_screen
    ui->keypad_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->keypad_screen, 480, 320);
    lv_obj_set_scrollbar_mode(ui->keypad_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for keypad_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->keypad_screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->keypad_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->keypad_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);


    //Write codes keypad_screen_wifi_img
    ui->keypad_screen_wifi_img = lv_image_create(ui->keypad_screen);
    lv_obj_set_pos(ui->keypad_screen_wifi_img, 345, 18);
    lv_obj_set_size(ui->keypad_screen_wifi_img, 38, 38);
    lv_obj_add_flag(ui->keypad_screen_wifi_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->keypad_screen_wifi_img, &_wifi_RGB565A8_38x38);
    lv_image_set_pivot(ui->keypad_screen_wifi_img, 50,50);
    lv_image_set_rotation(ui->keypad_screen_wifi_img, 0);

    //Write style for keypad_screen_wifi_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->keypad_screen_wifi_img, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->keypad_screen_wifi_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes keypad_screen_keypad
    ui->keypad_screen_keypad = lv_buttonmatrix_create(ui->keypad_screen);
    lv_obj_set_pos(ui->keypad_screen_keypad, 36, 39);
    lv_obj_set_size(ui->keypad_screen_keypad, 239, 256);
    static const char *keypad_screen_keypad_text_map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", " ", ".", "0", "DEL", "",};
    lv_buttonmatrix_set_map(ui->keypad_screen_keypad, keypad_screen_keypad_text_map);

    //Write style for keypad_screen_keypad, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->keypad_screen_keypad, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->keypad_screen_keypad, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->keypad_screen_keypad, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->keypad_screen_keypad, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->keypad_screen_keypad, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->keypad_screen_keypad, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->keypad_screen_keypad, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->keypad_screen_keypad, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->keypad_screen_keypad, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->keypad_screen_keypad, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->keypad_screen_keypad, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for keypad_screen_keypad, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->keypad_screen_keypad, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->keypad_screen_keypad, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->keypad_screen_keypad, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->keypad_screen_keypad, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->keypad_screen_keypad, lv_color_hex(0x000000), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->keypad_screen_keypad, &lv_font_Antonio_Regular_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->keypad_screen_keypad, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->keypad_screen_keypad, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->keypad_screen_keypad, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->keypad_screen_keypad, lv_color_hex(0xa5a5a5), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->keypad_screen_keypad, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->keypad_screen_keypad, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes keypad_screen_call_imgbtn
    ui->keypad_screen_call_imgbtn = lv_imagebutton_create(ui->keypad_screen);
    lv_obj_set_pos(ui->keypad_screen_call_imgbtn, 345, 177);
    lv_obj_set_size(ui->keypad_screen_call_imgbtn, 85, 85);
    lv_obj_add_flag(ui->keypad_screen_call_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->keypad_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_call_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->keypad_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_call_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->keypad_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_call_RGB565A8_85x85, NULL, NULL);
    lv_imagebutton_set_src(ui->keypad_screen_call_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_call_RGB565A8_85x85, NULL, NULL);
    ui->keypad_screen_call_imgbtn_label = lv_label_create(ui->keypad_screen_call_imgbtn);
    lv_label_set_text(ui->keypad_screen_call_imgbtn_label, "");
    lv_label_set_long_mode(ui->keypad_screen_call_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->keypad_screen_call_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->keypad_screen_call_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for keypad_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->keypad_screen_call_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->keypad_screen_call_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->keypad_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->keypad_screen_call_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->keypad_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for keypad_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->keypad_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->keypad_screen_call_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->keypad_screen_call_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->keypad_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->keypad_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for keypad_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->keypad_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->keypad_screen_call_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->keypad_screen_call_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->keypad_screen_call_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->keypad_screen_call_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for keypad_screen_call_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_call_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->keypad_screen_call_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes keypad_screen_num_label
    ui->keypad_screen_num_label = lv_label_create(ui->keypad_screen);
    lv_obj_set_pos(ui->keypad_screen_num_label, 338, 113);
    lv_obj_set_size(ui->keypad_screen_num_label, 100, 32);
    lv_label_set_text(ui->keypad_screen_num_label, "####");
    lv_label_set_long_mode(ui->keypad_screen_num_label, LV_LABEL_LONG_WRAP);

    //Write style for keypad_screen_num_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->keypad_screen_num_label, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->keypad_screen_num_label, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->keypad_screen_num_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->keypad_screen_num_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->keypad_screen_num_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes keypad_screen_back_imgbtn
    ui->keypad_screen_back_imgbtn = lv_imagebutton_create(ui->keypad_screen);
    lv_obj_set_pos(ui->keypad_screen_back_imgbtn, 57, 231);
    lv_obj_set_size(ui->keypad_screen_back_imgbtn, 35, 40);
    lv_obj_add_flag(ui->keypad_screen_back_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->keypad_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_35x40, NULL, NULL);
    lv_imagebutton_set_src(ui->keypad_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_PRESSED, &_back_RGB565A8_35x40, NULL, NULL);
    lv_imagebutton_set_src(ui->keypad_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_back_RGB565A8_35x40, NULL, NULL);
    lv_imagebutton_set_src(ui->keypad_screen_back_imgbtn, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_back_RGB565A8_35x40, NULL, NULL);
    ui->keypad_screen_back_imgbtn_label = lv_label_create(ui->keypad_screen_back_imgbtn);
    lv_label_set_text(ui->keypad_screen_back_imgbtn_label, "");
    lv_label_set_long_mode(ui->keypad_screen_back_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->keypad_screen_back_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->keypad_screen_back_imgbtn, 0, LV_STATE_DEFAULT);

    //Write style for keypad_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->keypad_screen_back_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->keypad_screen_back_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->keypad_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->keypad_screen_back_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->keypad_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for keypad_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->keypad_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->keypad_screen_back_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->keypad_screen_back_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->keypad_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->keypad_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for keypad_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->keypad_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->keypad_screen_back_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->keypad_screen_back_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->keypad_screen_back_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->keypad_screen_back_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for keypad_screen_back_imgbtn, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->keypad_screen_back_imgbtn, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->keypad_screen_back_imgbtn, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //The custom code of keypad_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->keypad_screen);

    //Init events for screen.
    events_init_keypad_screen(ui);
}
