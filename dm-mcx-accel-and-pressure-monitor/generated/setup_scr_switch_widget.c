/*
 * Copyright 2023-2024 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"
#include "sensors.h"


#define ACCEL_UNIT "\nmg"
#define TEMPERATURE_UNIT "degC"

volatile static uint8_t g_label_celcius[]            = "°C";
volatile static uint8_t g_label_fahrenheit[]         = "°F";

static char g_accel_str[]            = " 2000";
static char temperature_str[]        = "125";
static char pressure_str[]           = "101252";

//Future use - RTC block
volatile static uint8_t  day;
volatile static uint8_t  month;
volatile static uint16_t year;
volatile static uint8_t  hours;
volatile static uint8_t  minutes;
volatile static uint8_t  seconds;
volatile static uint8_t g_label_date[]                = "Sep 07, 2023";
volatile static uint8_t g_label_time[]                = "12:00:00 am";

// Accelerometer FXLS8974FC------------------------------------------------
volatile static int16_t g_xaxis_mg                      = 0;
volatile static int16_t g_yaxis_mg                      = 0;
volatile static int16_t g_zaxis_mg                      = 0;
volatile static int16_t g_vec_magnitude_mg              = 0;
volatile static int8_t g_accel_temperature              = 0;
// Chart data
volatile static int16_t x_axis_chart                    = 0;
volatile static int16_t y_axis_chart                    = 0;
volatile static int16_t z_axis_chart                    = 0;

//data array for charts
volatile static int16_t x_axis_array[100]				={0};
// ------------------------------------------------------------------------

// MPL3115 Pressure sensor variables
volatile static uint32_t MPL3115_PRESSURE_PA = 0;
volatile static uint8_t MPL3115_TEMP = 0.0;
volatile static uint8_t MPL3115_STATUS;
// ------------------------------------------------------------------------


volatile static sensor_updates_t g_sensors_state       = {0};
const uint8_t g_months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

temp_type_t temperature_type = temp_celsius;

void update_screen_values(lv_ui *ui)
{
	/*rtc_data_t                 rtc_data;*/
	fxls8974_data_t 		   fxls8974_sens;
	mpl3115_data_t 		   	   mpl3115_sens;
	fxls8974_chart_data_t      chart_data;

	if(true == sensor_updates_screen())
	{
		g_sensors_state = get_values_state_screen();

		if(true == g_sensors_state.update_fxls8974) /* Accel sensor new samples available */
		{
			fxls8974_sens = get_accel_data();

			g_xaxis_mg = fxls8974_sens.x_axis_mg;
			g_yaxis_mg = fxls8974_sens.y_axis_mg;
			g_zaxis_mg = fxls8974_sens.z_axis_mg;
			g_vec_magnitude_mg = fxls8974_sens.v_mag_mg;
			g_accel_temperature = fxls8974_sens.temperature;

			//x_axis
			snprintf(g_accel_str, sizeof(g_accel_str), "%5i", g_xaxis_mg);
			strcat(g_accel_str, ACCEL_UNIT);  //"Insert space char "
			lv_label_set_text(ui->switch_widget_x_axis_lbl, (char*)g_accel_str);

			//y_axis
			snprintf(g_accel_str, sizeof(g_accel_str), "%5i", g_yaxis_mg);
			strcat(g_accel_str, ACCEL_UNIT);  //"Insert space char "
			lv_label_set_text(ui->switch_widget_y_axis_lbl, (char*)g_accel_str);

			//z_axis
			snprintf(g_accel_str, sizeof(g_accel_str), "%5i", g_zaxis_mg);
			strcat(g_accel_str, ACCEL_UNIT);  //"Insert space char "
			lv_label_set_text(ui->switch_widget_z_axis_lbl, (char*)g_accel_str);

			//vector magnitude
			snprintf(g_accel_str, sizeof(g_accel_str), "%5i", g_vec_magnitude_mg);
			strcat(g_accel_str, ACCEL_UNIT);  //"Insert space char "
			lv_label_set_text(ui->switch_widget_vec_mag_lbl, (char*)g_accel_str);

			// Set temperature - fxl8974CF --------------------------
			get_temp_type(&temperature_type);
			if (temperature_type == temp_celsius)
			{
				/* Celsius */
				snprintf(temperature_str, sizeof(temperature_str), "%2i", g_accel_temperature);
				lv_label_set_text(ui->switch_widget_accel_temp_lbl, (char*)temperature_str);
				lv_label_set_text(ui->switch_widget_temp_type_lbl, (char*)g_label_celcius);

			}
			else if (temperature_type == temp_fahrenheit)
			{
				/* Fahrenheit */
				uint8_t temp_f;

				temp_f = (uint8_t)((g_accel_temperature * 1.8) + 32);
				snprintf(temperature_str, sizeof(temperature_str), "%3i", temp_f);
				lv_label_set_text(ui->switch_widget_accel_temp_lbl, (char*)temperature_str);
				lv_label_set_text(ui->switch_widget_temp_type_lbl, (char*)g_label_fahrenheit);
			}

			//chart functions
			chart_data = get_chart_data();
			x_axis_chart = chart_data.x_axis_chart;
			y_axis_chart = chart_data.y_axis_chart;
			z_axis_chart = chart_data.z_axis_chart;

			lv_chart_set_next_value(ui->switch_widget_chart_2,
					                ui->switch_widget_chart_2_0, x_axis_chart);

			//y axis line - chart
			lv_chart_set_next_value(ui->switch_widget_chart_2,
					                ui->switch_widget_chart_2_1, y_axis_chart);

			//z axis line - chart
			lv_chart_set_next_value(ui->switch_widget_chart_2,
					                ui->switch_widget_chart_2_2, z_axis_chart);
			//lv_chart_refresh(ui->switch_widget_chart_2); /*Required after direct set*/

			g_sensors_state.update_fxls8974 = false;
		}

		// Pressure sensor --------------------------------
		if(true == g_sensors_state.update_mpl3115) /* Pressure sensor new samples available */
		{
			mpl3115_sens = get_pressure_data();

			MPL3115_PRESSURE_PA = mpl3115_sens.pressure_pa;
			MPL3115_TEMP = mpl3115_sens.temperature;
			MPL3115_STATUS = mpl3115_sens.status;

			snprintf(pressure_str, sizeof(pressure_str), "%6i", MPL3115_PRESSURE_PA);
			lv_label_set_text(ui->switch_widget_pressure_temp_lbl, (char*)pressure_str);

			//Set meter indicator
			lv_meter_set_indicator_value(ui->switch_widget_meter_1,
					                     ui->switch_widget_meter_1_scale_0_ndline_0,
										 (uint8_t)(MPL3115_PRESSURE_PA/1000));

			g_sensors_state.update_mpl3115 = false;
		}
		// ------------------------------------------
	}
}

void setup_scr_switch_widget(lv_ui *ui)
{
	//Write codes switch_widget
	ui->switch_widget = lv_obj_create(NULL);
	lv_obj_set_size(ui->switch_widget, 480, 320);
	lv_obj_set_scrollbar_mode(ui->switch_widget, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->switch_widget, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget, LV_GRAD_DIR_VER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->switch_widget, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_main_stop(ui->switch_widget, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_stop(ui->switch_widget, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_tabview_1
	ui->switch_widget_tabview_1 = lv_tabview_create(ui->switch_widget, LV_DIR_TOP, 50);
	lv_obj_set_pos(ui->switch_widget_tabview_1, 0, 0);
	lv_obj_set_size(ui->switch_widget_tabview_1, 480, 320);
	lv_obj_set_scrollbar_mode(ui->switch_widget_tabview_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget_tabview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->switch_widget_tabview_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_tabview_1, lv_color_hex(0x3a3b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_tabview_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_tabview_1, lv_color_hex(0xa6a6a6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_tabview_1, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_tabview_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_tabview_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_tabview_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->switch_widget_tabview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_tabview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_tabview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_switch_widget_tabview_1_extra_btnm_main_default
	static lv_style_t style_switch_widget_tabview_1_extra_btnm_main_default;
	ui_init_style(&style_switch_widget_tabview_1_extra_btnm_main_default);
	
	lv_style_set_bg_opa(&style_switch_widget_tabview_1_extra_btnm_main_default, 255);
	lv_style_set_bg_color(&style_switch_widget_tabview_1_extra_btnm_main_default, lv_color_hex(0x060606));
	lv_style_set_bg_grad_dir(&style_switch_widget_tabview_1_extra_btnm_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_border_width(&style_switch_widget_tabview_1_extra_btnm_main_default, 0);
	lv_style_set_radius(&style_switch_widget_tabview_1_extra_btnm_main_default, 0);
	lv_obj_add_style(lv_tabview_get_tab_btns(ui->switch_widget_tabview_1), &style_switch_widget_tabview_1_extra_btnm_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_switch_widget_tabview_1_extra_btnm_items_default
	static lv_style_t style_switch_widget_tabview_1_extra_btnm_items_default;
	ui_init_style(&style_switch_widget_tabview_1_extra_btnm_items_default);
	
	lv_style_set_text_color(&style_switch_widget_tabview_1_extra_btnm_items_default, lv_color_hex(0xfafafa));
	lv_style_set_text_font(&style_switch_widget_tabview_1_extra_btnm_items_default, &lv_font_montserratMedium_15);
	lv_style_set_text_opa(&style_switch_widget_tabview_1_extra_btnm_items_default, 255);
	lv_obj_add_style(lv_tabview_get_tab_btns(ui->switch_widget_tabview_1), &style_switch_widget_tabview_1_extra_btnm_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_CHECKED for &style_switch_widget_tabview_1_extra_btnm_items_checked
	static lv_style_t style_switch_widget_tabview_1_extra_btnm_items_checked;
	ui_init_style(&style_switch_widget_tabview_1_extra_btnm_items_checked);
	
	lv_style_set_text_color(&style_switch_widget_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
	lv_style_set_text_font(&style_switch_widget_tabview_1_extra_btnm_items_checked, &lv_font_montserratMedium_12);
	lv_style_set_text_opa(&style_switch_widget_tabview_1_extra_btnm_items_checked, 255);
	lv_style_set_border_width(&style_switch_widget_tabview_1_extra_btnm_items_checked, 4);
	lv_style_set_border_opa(&style_switch_widget_tabview_1_extra_btnm_items_checked, 255);
	lv_style_set_border_color(&style_switch_widget_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
	lv_style_set_border_side(&style_switch_widget_tabview_1_extra_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
	lv_style_set_radius(&style_switch_widget_tabview_1_extra_btnm_items_checked, 0);
	lv_style_set_bg_opa(&style_switch_widget_tabview_1_extra_btnm_items_checked, 60);
	lv_style_set_bg_color(&style_switch_widget_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
	lv_style_set_bg_grad_dir(&style_switch_widget_tabview_1_extra_btnm_items_checked, LV_GRAD_DIR_NONE);
	lv_obj_add_style(lv_tabview_get_tab_btns(ui->switch_widget_tabview_1), &style_switch_widget_tabview_1_extra_btnm_items_checked, LV_PART_ITEMS|LV_STATE_CHECKED);

	//Write codes ACCEL
	ui->switch_widget_tabview_1_tab_1 = lv_tabview_add_tab(ui->switch_widget_tabview_1,"ACCEL");
	lv_obj_t * switch_widget_tabview_1_tab_1_label = lv_label_create(ui->switch_widget_tabview_1_tab_1);
	lv_label_set_text(switch_widget_tabview_1_tab_1_label, "FXLS8974 Accelerometer");

	//Write codes switch_widget_cont_1
	ui->switch_widget_cont_1 = lv_obj_create(ui->switch_widget_tabview_1_tab_1);
	lv_obj_set_pos(ui->switch_widget_cont_1, 53, 40);
	lv_obj_set_size(ui->switch_widget_cont_1, 80, 80);
	lv_obj_set_scrollbar_mode(ui->switch_widget_cont_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_cont_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->switch_widget_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->switch_widget_cont_1, lv_color_hex(0x2f3339), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->switch_widget_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_cont_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_cont_1, lv_color_hex(0xffdb00), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_12
	ui->switch_widget_label_12 = lv_label_create(ui->switch_widget_cont_1);
	lv_label_set_text(ui->switch_widget_label_12, "Xaxis");
	lv_label_set_long_mode(ui->switch_widget_label_12, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_12, 18, 10);
	lv_obj_set_size(ui->switch_widget_label_12, 50, 18);

	//Write style for switch_widget_label_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_12, lv_color_hex(0x7c0404), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_12, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_12, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_12, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_x_axis_lbl
	ui->switch_widget_x_axis_lbl = lv_label_create(ui->switch_widget_cont_1);
	lv_label_set_text(ui->switch_widget_x_axis_lbl, "2000\nmg");
	lv_label_set_long_mode(ui->switch_widget_x_axis_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_x_axis_lbl, 11, 38);
	lv_obj_set_size(ui->switch_widget_x_axis_lbl, 61, 39);

	//Write style for switch_widget_x_axis_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_x_axis_lbl, lv_color_hex(0x2c2929), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_x_axis_lbl, &lv_font_Amiko_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_x_axis_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_x_axis_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_x_axis_lbl, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_x_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_accel_temp_lbl
	ui->switch_widget_accel_temp_lbl = lv_label_create(ui->switch_widget_tabview_1_tab_1);
	lv_label_set_text(ui->switch_widget_accel_temp_lbl, "25");
	lv_label_set_long_mode(ui->switch_widget_accel_temp_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_accel_temp_lbl, 250, 154);
	lv_obj_set_size(ui->switch_widget_accel_temp_lbl, 140, 76);

	//Write style for switch_widget_accel_temp_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_accel_temp_lbl, lv_color_hex(0x2ebd07), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_accel_temp_lbl, &lv_font_Amiko_Regular_72, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_accel_temp_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_accel_temp_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_accel_temp_lbl, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_accel_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_temp_type_lbl
	ui->switch_widget_temp_type_lbl = lv_label_create(ui->switch_widget_tabview_1_tab_1);
	lv_label_set_text(ui->switch_widget_temp_type_lbl, "°C");
	lv_label_set_long_mode(ui->switch_widget_temp_type_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_temp_type_lbl, 392, 174);
	lv_obj_set_size(ui->switch_widget_temp_type_lbl, 50, 37);

	//Write style for switch_widget_temp_type_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_temp_type_lbl, lv_color_hex(0x2ebd07), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_temp_type_lbl, &lv_font_montserratMedium_36, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_temp_type_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_temp_type_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_temp_type_lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_temp_type_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_cont_2
	ui->switch_widget_cont_2 = lv_obj_create(ui->switch_widget_tabview_1_tab_1);
	lv_obj_set_pos(ui->switch_widget_cont_2, 150, 40);
	lv_obj_set_size(ui->switch_widget_cont_2, 80, 80);
	lv_obj_set_scrollbar_mode(ui->switch_widget_cont_2, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_cont_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->switch_widget_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->switch_widget_cont_2, lv_color_hex(0x2f3339), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->switch_widget_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_cont_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_cont_2, lv_color_hex(0xffdb00), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_13
	ui->switch_widget_label_13 = lv_label_create(ui->switch_widget_cont_2);
	lv_label_set_text(ui->switch_widget_label_13, "Yaxis");
	lv_label_set_long_mode(ui->switch_widget_label_13, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_13, 18, 10);
	lv_obj_set_size(ui->switch_widget_label_13, 50, 18);

	//Write style for switch_widget_label_13, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_13, lv_color_hex(0x7c0404), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_13, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_13, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_13, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_13, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_13, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_y_axis_lbl
	ui->switch_widget_y_axis_lbl = lv_label_create(ui->switch_widget_cont_2);
	lv_label_set_text(ui->switch_widget_y_axis_lbl, "2000\nmg");
	lv_label_set_long_mode(ui->switch_widget_y_axis_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_y_axis_lbl, 11, 38);
	lv_obj_set_size(ui->switch_widget_y_axis_lbl, 61, 39);

	//Write style for switch_widget_y_axis_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_y_axis_lbl, lv_color_hex(0x2c2929), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_y_axis_lbl, &lv_font_Amiko_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_y_axis_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_y_axis_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_y_axis_lbl, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_y_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_cont_4
	ui->switch_widget_cont_4 = lv_obj_create(ui->switch_widget_tabview_1_tab_1);
	lv_obj_set_pos(ui->switch_widget_cont_4, 150, 136);
	lv_obj_set_size(ui->switch_widget_cont_4, 80, 80);
	lv_obj_set_scrollbar_mode(ui->switch_widget_cont_4, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_cont_4, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->switch_widget_cont_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->switch_widget_cont_4, lv_color_hex(0x2f3339), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->switch_widget_cont_4, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_cont_4, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_cont_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_cont_4, lv_color_hex(0x00ffee), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_cont_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_vec_mag_lbl
	ui->switch_widget_vec_mag_lbl = lv_label_create(ui->switch_widget_cont_4);
	lv_label_set_text(ui->switch_widget_vec_mag_lbl, "2000\nmg");
	lv_label_set_long_mode(ui->switch_widget_vec_mag_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_vec_mag_lbl, 11, 38);
	lv_obj_set_size(ui->switch_widget_vec_mag_lbl, 61, 39);

	//Write style for switch_widget_vec_mag_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_vec_mag_lbl, lv_color_hex(0x2c2929), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_vec_mag_lbl, &lv_font_Amiko_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_vec_mag_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_vec_mag_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_vec_mag_lbl, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_vec_mag_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_22
	ui->switch_widget_label_22 = lv_label_create(ui->switch_widget_cont_4);
	lv_label_set_text(ui->switch_widget_label_22, "Vector");
	lv_label_set_long_mode(ui->switch_widget_label_22, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_22, 10, 10);
	lv_obj_set_size(ui->switch_widget_label_22, 60, 18);

	//Write style for switch_widget_label_22, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_22, lv_color_hex(0x7c0404), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_22, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_22, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_22, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_22, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_cont_3
	ui->switch_widget_cont_3 = lv_obj_create(ui->switch_widget_tabview_1_tab_1);
	lv_obj_set_pos(ui->switch_widget_cont_3, 53, 136);
	lv_obj_set_size(ui->switch_widget_cont_3, 80, 80);
	lv_obj_set_scrollbar_mode(ui->switch_widget_cont_3, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_cont_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->switch_widget_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->switch_widget_cont_3, lv_color_hex(0x2f3339), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->switch_widget_cont_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_cont_3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_cont_3, lv_color_hex(0xffdb00), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_lbl3
	ui->switch_widget_lbl3 = lv_label_create(ui->switch_widget_cont_3);
	lv_label_set_text(ui->switch_widget_lbl3, "Zaxis");
	lv_label_set_long_mode(ui->switch_widget_lbl3, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_lbl3, 18, 10);
	lv_obj_set_size(ui->switch_widget_lbl3, 50, 18);

	//Write style for switch_widget_lbl3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_lbl3, lv_color_hex(0x7c0404), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_lbl3, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_lbl3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_lbl3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_lbl3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_lbl3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_z_axis_lbl
	ui->switch_widget_z_axis_lbl = lv_label_create(ui->switch_widget_cont_3);
	lv_label_set_text(ui->switch_widget_z_axis_lbl, "2000\nmg");
	lv_label_set_long_mode(ui->switch_widget_z_axis_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_z_axis_lbl, 11, 38);
	lv_obj_set_size(ui->switch_widget_z_axis_lbl, 61, 39);

	//Write style for switch_widget_z_axis_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_z_axis_lbl, lv_color_hex(0x2c2929), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_z_axis_lbl, &lv_font_Amiko_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_z_axis_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_z_axis_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_z_axis_lbl, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_z_axis_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_img_2
	ui->switch_widget_img_2 = lv_img_create(ui->switch_widget_tabview_1_tab_1);
	lv_obj_add_flag(ui->switch_widget_img_2, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->switch_widget_img_2, &_temp_sw_alpha_30x27);
	lv_img_set_pivot(ui->switch_widget_img_2, 65,65);
	lv_img_set_angle(ui->switch_widget_img_2, 0);
	lv_obj_set_pos(ui->switch_widget_img_2, 415, 0);
	lv_obj_set_size(ui->switch_widget_img_2, 30, 27);

	//Write style for switch_widget_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->switch_widget_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes PRESSURE
	ui->switch_widget_tabview_1_tab_2 = lv_tabview_add_tab(ui->switch_widget_tabview_1,"PRESSURE");
	lv_obj_t * switch_widget_tabview_1_tab_2_label = lv_label_create(ui->switch_widget_tabview_1_tab_2);
	lv_label_set_text(switch_widget_tabview_1_tab_2_label, "MPL3115 Pressure Sensor");

	//Write codes switch_widget_meter_1
	ui->switch_widget_meter_1 = lv_meter_create(ui->switch_widget_tabview_1_tab_2);
	// add scale switch_widget_meter_1_scale_0
	lv_meter_scale_t *switch_widget_meter_1_scale_0 = lv_meter_add_scale(ui->switch_widget_meter_1);
	lv_meter_set_scale_ticks(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, 110, 2, 8, lv_color_hex(0x8d8a8a));
	lv_meter_set_scale_major_ticks(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, 10, 2, 16, lv_color_hex(0x737373), 10);
	lv_meter_set_scale_range(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, 0, 110, 280, 120);

	// add arc for switch_widget_meter_1_scale_0
	ui->switch_widget_meter_1_scale_0_arc_0 = lv_meter_add_arc(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, 2, lv_color_hex(0x4b4445), 0);
	lv_meter_set_indicator_start_value(ui->switch_widget_meter_1, ui->switch_widget_meter_1_scale_0_arc_0, 0);
	lv_meter_set_indicator_end_value(ui->switch_widget_meter_1, ui->switch_widget_meter_1_scale_0_arc_0, 200);

	// add scale line for switch_widget_meter_1_scale_0
	lv_meter_indicator_t *switch_widget_meter_1_scale_0_scaleline_0;
	switch_widget_meter_1_scale_0_scaleline_0 = lv_meter_add_scale_lines(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, lv_color_hex(0xd60000), lv_color_hex(0xd60000), false, 0);
	lv_meter_set_indicator_start_value(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0_scaleline_0, 0);
	lv_meter_set_indicator_end_value(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0_scaleline_0, 20);

	// add scale line for switch_widget_meter_1_scale_0
	lv_meter_indicator_t *switch_widget_meter_1_scale_0_scaleline_1;
	switch_widget_meter_1_scale_0_scaleline_1 = lv_meter_add_scale_lines(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, lv_color_hex(0xede501), lv_color_hex(0xede501), false, 0);
	lv_meter_set_indicator_start_value(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0_scaleline_1, 20);
	lv_meter_set_indicator_end_value(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0_scaleline_1, 90);

	// add scale line for switch_widget_meter_1_scale_0
	lv_meter_indicator_t *switch_widget_meter_1_scale_0_scaleline_2;
	switch_widget_meter_1_scale_0_scaleline_2 = lv_meter_add_scale_lines(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, lv_color_hex(0x14bd22), lv_color_hex(0x14bd22), false, 0);
	lv_meter_set_indicator_start_value(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0_scaleline_2, 80);
	lv_meter_set_indicator_end_value(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0_scaleline_2, 111);

	// add needle line for switch_widget_meter_1_scale_0.
	ui->switch_widget_meter_1_scale_0_ndline_0 = lv_meter_add_needle_line(ui->switch_widget_meter_1, switch_widget_meter_1_scale_0, 2, lv_color_hex(0xffb100), -20);
	lv_meter_set_indicator_value(ui->switch_widget_meter_1, ui->switch_widget_meter_1_scale_0_ndline_0, 101);
	lv_obj_set_pos(ui->switch_widget_meter_1, 36, 26);
	lv_obj_set_size(ui->switch_widget_meter_1, 200, 200);

	//Write style for switch_widget_meter_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->switch_widget_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_meter_1, 200, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->switch_widget_meter_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->switch_widget_meter_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->switch_widget_meter_1, lv_color_hex(0x616161), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->switch_widget_meter_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for switch_widget_meter_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->switch_widget_meter_1, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_meter_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_meter_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write style for switch_widget_meter_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->switch_widget_meter_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_meter_1, lv_color_hex(0xa17001), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_meter_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_20
	ui->switch_widget_label_20 = lv_label_create(ui->switch_widget_tabview_1_tab_2);
	lv_label_set_text(ui->switch_widget_label_20, "Pa");
	lv_label_set_long_mode(ui->switch_widget_label_20, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_20, 381, 193);
	lv_obj_set_size(ui->switch_widget_label_20, 64, 24);

	//Write style for switch_widget_label_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_20, lv_color_hex(0xf2b500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_20, &lv_font_Amiko_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_20, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_20, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_20, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_pressure_temp_lbl
	ui->switch_widget_pressure_temp_lbl = lv_label_create(ui->switch_widget_tabview_1_tab_2);
	lv_label_set_text(ui->switch_widget_pressure_temp_lbl, "101585");
	lv_label_set_long_mode(ui->switch_widget_pressure_temp_lbl, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_pressure_temp_lbl, 232, 181);
	lv_obj_set_size(ui->switch_widget_pressure_temp_lbl, 176, 48);

	//Write style for switch_widget_pressure_temp_lbl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_pressure_temp_lbl, lv_color_hex(0xf2b500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_pressure_temp_lbl, &lv_font_Amiko_Regular_42, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_pressure_temp_lbl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_pressure_temp_lbl, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_pressure_temp_lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_pressure_temp_lbl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_21
	ui->switch_widget_label_21 = lv_label_create(ui->switch_widget_tabview_1_tab_2);
	lv_label_set_text(ui->switch_widget_label_21, "kPa");
	lv_label_set_long_mode(ui->switch_widget_label_21, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_21, 115, 138);
	lv_obj_set_size(ui->switch_widget_label_21, 49, 24);

	//Write style for switch_widget_label_21, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_21, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_21, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_21, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_21, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_21, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes CHART
	ui->switch_widget_tabview_1_tab_3 = lv_tabview_add_tab(ui->switch_widget_tabview_1,"CHART");
	lv_obj_t * switch_widget_tabview_1_tab_3_label = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(switch_widget_tabview_1_tab_3_label, "Accelerometer Data Chart");

	//Write codes switch_widget_chart_2
	ui->switch_widget_chart_2 = lv_chart_create(ui->switch_widget_tabview_1_tab_3);
	lv_chart_set_type(ui->switch_widget_chart_2, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(ui->switch_widget_chart_2, 5, 5);
	lv_chart_set_point_count(ui->switch_widget_chart_2, 40);
	lv_chart_set_range(ui->switch_widget_chart_2, LV_CHART_AXIS_PRIMARY_Y, -2000, 2000);
	lv_chart_set_axis_tick(ui->switch_widget_chart_2, LV_CHART_AXIS_PRIMARY_Y, 12, 6, 9, 4, true, 65);
	lv_chart_set_range(ui->switch_widget_chart_2, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
	lv_chart_set_zoom_x(ui->switch_widget_chart_2, 256);
	lv_chart_set_zoom_y(ui->switch_widget_chart_2, 256);
	ui->switch_widget_chart_2_0 = lv_chart_add_series(ui->switch_widget_chart_2, lv_color_hex(0xeb1212), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
	lv_chart_set_next_value(ui->switch_widget_chart_2, ui->switch_widget_chart_2_0, 0);
#endif
	ui->switch_widget_chart_2_1 = lv_chart_add_series(ui->switch_widget_chart_2, lv_color_hex(0x2c7dd0), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
	lv_chart_set_next_value(ui->switch_widget_chart_2, ui->switch_widget_chart_2_1, 0);
#endif
	ui->switch_widget_chart_2_2 = lv_chart_add_series(ui->switch_widget_chart_2, lv_color_hex(0x11f20b), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
	lv_chart_set_next_value(ui->switch_widget_chart_2, ui->switch_widget_chart_2_2, 0);
#endif
	lv_obj_set_pos(ui->switch_widget_chart_2, 69, 32);
	lv_obj_set_size(ui->switch_widget_chart_2, 271, 190);
	lv_obj_set_scrollbar_mode(ui->switch_widget_chart_2, LV_SCROLLBAR_MODE_OFF);

	//Write style for switch_widget_chart_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->switch_widget_chart_2, 83, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_chart_2, lv_color_hex(0xbcbcb9), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_chart_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->switch_widget_chart_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->switch_widget_chart_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->switch_widget_chart_2, lv_color_hex(0x6c6c6c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->switch_widget_chart_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_chart_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->switch_widget_chart_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->switch_widget_chart_2, lv_color_hex(0x989997), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->switch_widget_chart_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_chart_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for switch_widget_chart_2, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->switch_widget_chart_2, lv_color_hex(0xc8c6c6), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_chart_2, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_chart_2, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->switch_widget_chart_2, 2, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->switch_widget_chart_2, lv_color_hex(0x656262), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->switch_widget_chart_2, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_23
	ui->switch_widget_label_23 = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(ui->switch_widget_label_23, "");
	lv_label_set_long_mode(ui->switch_widget_label_23, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_23, 353, 40);
	lv_obj_set_size(ui->switch_widget_label_23, 10, 10);

	//Write style for switch_widget_label_23, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_23, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_23, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_23, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_23, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_23, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_label_23, lv_color_hex(0xf57e7e), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_label_23, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_23, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_24
	ui->switch_widget_label_24 = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(ui->switch_widget_label_24, "Y Axis (mg)");
	lv_label_set_long_mode(ui->switch_widget_label_24, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_24, 370, 62);
	lv_obj_set_size(ui->switch_widget_label_24, 69, 19);

	//Write style for switch_widget_label_24, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_24, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_24, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_24, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_24, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_24, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_25
	ui->switch_widget_label_25 = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(ui->switch_widget_label_25, "Z Axis (mg)");
	lv_label_set_long_mode(ui->switch_widget_label_25, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_25, 370, 89);
	lv_obj_set_size(ui->switch_widget_label_25, 69, 19);

	//Write style for switch_widget_label_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_25, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_25, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_25, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_25, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_25, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_26
	ui->switch_widget_label_26 = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(ui->switch_widget_label_26, "X Axis (mg)");
	lv_label_set_long_mode(ui->switch_widget_label_26, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_26, 370, 34);
	lv_obj_set_size(ui->switch_widget_label_26, 69, 19);

	//Write style for switch_widget_label_26, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_26, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_26, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_26, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_26, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_26, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_26, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_27
	ui->switch_widget_label_27 = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(ui->switch_widget_label_27, "");
	lv_label_set_long_mode(ui->switch_widget_label_27, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_27, 353, 67);
	lv_obj_set_size(ui->switch_widget_label_27, 10, 10);

	//Write style for switch_widget_label_27, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_27, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_27, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_27, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_27, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_27, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_label_27, lv_color_hex(0x7ec4fd), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_label_27, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_27, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_28
	ui->switch_widget_label_28 = lv_label_create(ui->switch_widget_tabview_1_tab_3);
	lv_label_set_text(ui->switch_widget_label_28, "");
	lv_label_set_long_mode(ui->switch_widget_label_28, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_28, 353, 94);
	lv_obj_set_size(ui->switch_widget_label_28, 10, 10);

	//Write style for switch_widget_label_28, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_28, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_28, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_28, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_28, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_28, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->switch_widget_label_28, lv_color_hex(0xa4ffa9), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->switch_widget_label_28, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_28, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ABOUT
	ui->switch_widget_tabview_1_tab_4 = lv_tabview_add_tab(ui->switch_widget_tabview_1,"ABOUT");
	lv_obj_t * switch_widget_tabview_1_tab_4_label = lv_label_create(ui->switch_widget_tabview_1_tab_4);
	lv_label_set_text(switch_widget_tabview_1_tab_4_label, "General Information");

	//Write codes switch_widget_label_10
	ui->switch_widget_label_10 = lv_label_create(ui->switch_widget_tabview_1_tab_4);
	lv_label_set_text(ui->switch_widget_label_10, "BOARD: \nMCU:\nSENSORS:\nLCD DISPLAY:\nDATE:\nSW VER:\n\n");
	lv_label_set_long_mode(ui->switch_widget_label_10, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_10, 17, 37);
	lv_obj_set_size(ui->switch_widget_label_10, 162, 87);

	//Write style for switch_widget_label_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_10, lv_color_hex(0x48d6ea), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_10, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_10, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_10, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_11
	ui->switch_widget_label_11 = lv_label_create(ui->switch_widget_tabview_1_tab_4);
	lv_label_set_text(ui->switch_widget_label_11, "FRDM-MCXN947\nMCXN947\nMPL3115A2S, FXLS8974CF\nLCD-PAR-S035\n04/12/2024\n1.0\n");
	lv_label_set_long_mode(ui->switch_widget_label_11, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_11, 172, 37);
	lv_obj_set_size(ui->switch_widget_label_11, 269, 86);

	//Write style for switch_widget_label_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_11, lv_color_hex(0xc3871c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_11, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_11, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_11, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes switch_widget_label_29
	ui->switch_widget_label_29 = lv_label_create(ui->switch_widget_tabview_1_tab_4);
	lv_label_set_text(ui->switch_widget_label_29, "NXP Sensors");
	lv_label_set_long_mode(ui->switch_widget_label_29, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->switch_widget_label_29, 17, 130);
	lv_obj_set_size(ui->switch_widget_label_29, 270, 34);

	//Write style for switch_widget_label_29, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->switch_widget_label_29, lv_color_hex(0x8d8c8c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->switch_widget_label_29, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->switch_widget_label_29, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->switch_widget_label_29, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->switch_widget_label_29, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->switch_widget_label_29, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->switch_widget_label_29, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of switch_widget.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->switch_widget);

	//Init events for screen.
	events_init_switch_widget(ui);
}
