/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>
#include <stdbool.h>
#include "fsl_lpadc.h"

#define INT_TO_CHAR 48
#define CHAR_TO_INT -48
#define MOVING_AVERAGE_ENABLE 1 /* Computes moving average over the last 10 samples captured */

typedef enum _sign_of_string_t
{
	positive,
	negative
} sign_of_string_t;

typedef struct _rtc_data_t
{
	uint8_t  day;
	uint8_t  month;
	uint16_t year;
	uint8_t  hours;
	uint8_t  minutes;
	uint8_t  seconds;
} rtc_data_t;

typedef struct _sensor_updates_t
{
	bool update_rtc;
	bool update_mpl3115;
	bool update_fxls8974;
} sensor_updates_t;

//Accelerometer data
typedef struct _fxls8974_data_t
{
	int16_t x_axis_mg;
	int16_t y_axis_mg;
	int16_t z_axis_mg;
	int16_t v_mag_mg;
	int8_t temperature;
	uint8_t status;
} fxls8974_data_t;

//Pressure sensor data
typedef struct _mpl3115_data_t
{
	uint32_t pressure_pa;
	int8_t temperature;
	uint8_t status;
} mpl3115_data_t;

//Chart accelerometer data
typedef struct _fxls8974_chart_data_t
{
	int16_t x_axis_chart;
	int16_t y_axis_chart;
	int16_t z_axis_chart;
} fxls8974_chart_data_t;


/********************************************************************************************
	@brief : This function convert integer to string
	@param : value   -> Integer value to convert
	@param : string  -> Buffer pointer to save string number
	@param : int_len -> String length, if is more than number need is filled with '0'
	@return: Return sign of number (Not put '-' in string)
*********************************************************************************************/
sign_of_string_t           int_to_string(int32_t value, uint8_t* string, uint8_t int_len);
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function convert string to integer.
	@param : buffer -> String to convert, support negative number "-00050" or "-50" or "50"
	@param : size   -> Size of string
	@return: Integer number
*********************************************************************************************/
int32_t                    string_to_int(uint8_t buffer[], uint8_t size);
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function is for lvgl, if some value is updated this returns true
	@return: True if some value is updated, false if not
*********************************************************************************************/
bool 					   sensor_updates_screen();
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This indicate values that are updated for lvgl (used for performance)
	@return: struct with flags that indicate updated values
*********************************************************************************************/
sensor_updates_t 		   get_values_state_screen();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of real time clock (RTC)
	@return: struct with all RTC values
*********************************************************************************************/
rtc_data_t                 get_rtc_data();  /* To be integrated next version */
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Periodic function used for update all values, called for "other" task in main
	@return: None
*********************************************************************************************/
void update_values();
// ------------------------------------------------------------------------------------------

bool update_pressure();

fxls8974_data_t get_accel_data(void);
mpl3115_data_t get_pressure_data(void);
fxls8974_chart_data_t get_chart_data(void);
float get_accel_mg(uint16_t accel_read);

float Moving_Average(float* X_ACCEL_ARRAY, float ACCEL_X);


#endif /* SENSORS_H_ */
