/*
* Copyright 2024 NXP
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
  
	lv_obj_t *switch_widget;
	bool switch_widget_del;
	lv_obj_t *switch_widget_tabview_1;
	lv_obj_t *switch_widget_tabview_1_tab_1;
	lv_obj_t *switch_widget_tabview_1_tab_2;
	lv_obj_t *switch_widget_tabview_1_tab_3;
	lv_obj_t *switch_widget_tabview_1_tab_4;
	lv_obj_t *switch_widget_cont_1;
	lv_obj_t *switch_widget_label_12;
	lv_obj_t *switch_widget_x_axis_lbl;
	lv_obj_t *switch_widget_accel_temp_lbl;
	lv_obj_t *switch_widget_temp_type_lbl;
	lv_obj_t *switch_widget_cont_2;
	lv_obj_t *switch_widget_label_13;
	lv_obj_t *switch_widget_y_axis_lbl;
	lv_obj_t *switch_widget_cont_4;
	lv_obj_t *switch_widget_vec_mag_lbl;
	lv_obj_t *switch_widget_label_22;
	lv_obj_t *switch_widget_cont_3;
	lv_obj_t *switch_widget_lbl3;
	lv_obj_t *switch_widget_z_axis_lbl;
	lv_obj_t *switch_widget_img_2;
	lv_obj_t *switch_widget_meter_1;
	lv_meter_indicator_t *switch_widget_meter_1_scale_0_ndline_0;
	lv_meter_indicator_t *switch_widget_meter_1_scale_0_arc_0;
	lv_obj_t *switch_widget_label_20;
	lv_obj_t *switch_widget_pressure_temp_lbl;
	lv_obj_t *switch_widget_label_21;
	lv_obj_t *switch_widget_chart_2;
	lv_chart_series_t *switch_widget_chart_2_0;
	lv_chart_series_t *switch_widget_chart_2_1;
	lv_chart_series_t *switch_widget_chart_2_2;
	lv_obj_t *switch_widget_label_23;
	lv_obj_t *switch_widget_label_24;
	lv_obj_t *switch_widget_label_25;
	lv_obj_t *switch_widget_label_26;
	lv_obj_t *switch_widget_label_27;
	lv_obj_t *switch_widget_label_28;
	lv_obj_t *switch_widget_label_10;
	lv_obj_t *switch_widget_label_11;
	lv_obj_t *switch_widget_img_1;
	lv_obj_t *switch_widget_label_29;
  int global_language;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_move_animation(void * var, int32_t duration, int32_t delay, int32_t x_end, int32_t y_end, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_scale_animation(void * var, int32_t duration, int32_t delay, int32_t width, int32_t height, lv_anim_path_cb_t path_cb,
                        uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                        lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_img_zoom_animation(void * var, int32_t duration, int32_t delay, int32_t zoom, lv_anim_path_cb_t path_cb,
                           uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                           lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_img_rotate_animation(void * var, int32_t duration, int32_t delay, lv_coord_t x, lv_coord_t y, int32_t rotate,
                   lv_anim_path_cb_t path_cb, uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time,
                   uint32_t playback_delay, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);



extern lv_ui guider_ui;
extern int global_language;

void update_screen_values(lv_ui *ui);  /* Function called in main function -- lvgl_guider.c */

void setup_scr_switch_widget(lv_ui *ui);
LV_IMG_DECLARE(_temp_sw_alpha_30x27);
LV_IMG_DECLARE(_NXP_logo_dark_alpha_143x51);

LV_FONT_DECLARE(lv_font_montserratMedium_14)
LV_FONT_DECLARE(lv_font_montserratMedium_15)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_Amiko_Regular_18)
LV_FONT_DECLARE(lv_font_Amiko_Regular_72)
LV_FONT_DECLARE(lv_font_montserratMedium_36)
LV_FONT_DECLARE(lv_font_Amiko_Regular_24)
LV_FONT_DECLARE(lv_font_Amiko_Regular_42)
LV_FONT_DECLARE(lv_font_montserratMedium_10)


#ifdef __cplusplus
}
#endif
#endif
