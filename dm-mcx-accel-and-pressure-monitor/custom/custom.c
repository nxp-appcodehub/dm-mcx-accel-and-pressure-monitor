/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

void widgets_move()
{

    	lv_anim_t switch_widget_img_guider_anim_x;

    	//Write animation: switch_widget_img_guider move in x direction
		lv_anim_init(&switch_widget_img_guider_anim_x);
		lv_anim_set_time(&switch_widget_img_guider_anim_x, 1000);
		lv_anim_set_exec_cb(&switch_widget_img_guider_anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
		lv_anim_set_path_cb(&switch_widget_img_guider_anim_x, &lv_anim_path_bounce);
		lv_anim_start(&switch_widget_img_guider_anim_x);

	//Write animation: switch_widget_img_guider move in y direction
		/*lv_anim_init(&switch_widget_img_guider_anim_y);
		lv_anim_set_time(&switch_widget_img_guider_anim_y, 1000);
		lv_anim_set_exec_cb(&switch_widget_img_guider_anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
		lv_anim_set_path_cb(&switch_widget_img_guider_anim_y, &lv_anim_path_bounce);
		lv_anim_start(&switch_widget_img_guider_anim_y);
		lv_obj_clear_flag(guider_ui.switch_widget_qrcode_community, LV_OBJ_FLAG_HIDDEN);
*/
	    //Write animation: switch_widget_community move in x direction
	/*	lv_anim_init(&switch_widget_community_anim_x);
		lv_anim_set_var(&switch_widget_community_anim_x, guider_ui.switch_widget_qrcode_community);
		lv_anim_set_time(&switch_widget_community_anim_x, 1000);
		lv_anim_set_exec_cb(&switch_widget_community_anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
		//Compatible with small size screens
		if(LV_HOR_RES_MAX < 480) {
			lv_anim_set_values(&switch_widget_community_anim_x, lv_obj_get_x(guider_ui.switch_widget_qrcode_community), 220 * LV_VER_RES_MAX / 272);
		} else {
			lv_anim_set_values(&switch_widget_community_anim_x, lv_obj_get_x(guider_ui.switch_widget_qrcode_community), 300 * LV_VER_RES_MAX / 272);
		}
		lv_anim_set_path_cb(&switch_widget_community_anim_x, &lv_anim_path_overshoot);
		lv_anim_start(&switch_widget_community_anim_x);

	    //Write animation: switch_widget_community move in y direction
		lv_anim_init(&switch_widget_community_anim_y);
		lv_anim_set_var(&switch_widget_community_anim_y, guider_ui.switch_widget_qrcode_community);
		lv_anim_set_time(&switch_widget_community_anim_y, 1000);
		lv_anim_set_exec_cb(&switch_widget_community_anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
		lv_anim_set_values(&switch_widget_community_anim_y, lv_obj_get_y(guider_ui.switch_widget_qrcode_community), 62 * LV_HOR_RES_MAX / 480);
		lv_anim_set_path_cb(&switch_widget_community_anim_y, &lv_anim_path_overshoot);
		lv_anim_start(&switch_widget_community_anim_y);*/
/*
        if(!lv_obj_has_state(guider_ui.switch_widget_sw_switch, LV_STATE_CHECKED)) {
			lv_anim_init(&switch_widget_img_guider_anim_x);
			lv_anim_set_time(&switch_widget_img_guider_anim_x, 1000);
			lv_anim_set_exec_cb(&switch_widget_img_guider_anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
			lv_anim_set_path_cb(&switch_widget_img_guider_anim_x, &lv_anim_path_bounce);
			lv_anim_start(&switch_widget_img_guider_anim_x);

			lv_anim_init(&switch_widget_community_anim_x);
			lv_anim_set_var(&switch_widget_community_anim_x, guider_ui.switch_widget_qrcode_community);
			lv_anim_set_time(&switch_widget_community_anim_x, 1000);
			lv_anim_set_exec_cb(&switch_widget_community_anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
			lv_anim_set_values(&switch_widget_community_anim_x, lv_obj_get_x(guider_ui.switch_widget_qrcode_community), 102 * LV_VER_RES_MAX / 272);
			lv_anim_set_path_cb(&switch_widget_community_anim_x, &lv_anim_path_overshoot);
			lv_anim_start(&switch_widget_community_anim_x);

			lv_obj_add_flag(guider_ui.switch_widget_qrcode_community, LV_OBJ_FLAG_HIDDEN);
        }
  */
}
