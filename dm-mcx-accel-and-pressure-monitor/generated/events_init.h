/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 */


#ifndef EVENTS_INIT_H_
#define EVENTS_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

void events_init(lv_ui *ui);

void events_init_switch_widget(lv_ui *ui);


typedef enum _temp_type {
  temp_celsius = 0,
  temp_fahrenheit
} temp_type_t;

void get_temp_type(temp_type_t *temp);


#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
