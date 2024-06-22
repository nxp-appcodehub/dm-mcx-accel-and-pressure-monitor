/*
 * Copyright 2023-2024 NXP
 * SPDX-License-Identifier: MIT
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


temp_type_t temp_type = temp_celsius;

/* Get the temperature type 
   Returns: temp_type */
void get_temp_type(temp_type_t *temp)
{
   *temp = temp_type;
}



static void switch_widget_img_2_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		
	if (temp_type == temp_celsius){
	  temp_type = temp_fahrenheit;
	}
	else if (temp_type == temp_fahrenheit){
	  temp_type = temp_celsius;
	}
	
		break;
	}
	default:
		break;
	}
}
void events_init_switch_widget(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->switch_widget_img_2, switch_widget_img_2_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{

}
