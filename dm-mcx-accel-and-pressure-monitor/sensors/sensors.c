/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sensors.h"
#include "fxls8974.h"
#include "mpl3115.h"
#include <i2c_app.h>
#include "systick_utils.h"
#include "FreeRTOS.h"
#include "task.h"

#define CELCIUS_TO_FAHRENHEIT(celcius) (((celcius)*1.8)+32)
#define AVE_BUFFER_LENGHT 10U


volatile static uint8_t  g_control_interface_counter   = 0;
volatile static uint16_t g_rpm                        = 0;

// Accelerometer FXLS8974FC------------------------------------------------
volatile static uint16_t g_xaxis               = 0;
volatile static uint16_t g_yaxis             = 0;
volatile static uint16_t g_zaxis             = 0;
volatile static uint16_t g_vec_magnitude             = 0;
volatile static uint8_t g_accel_temperature             = 0;
volatile static bool    g_accel_data_updated         = false;
// ------------------------------------------------------------------------

// Temperature ------------------------------------------------------------
volatile static uint8_t g_temperature_c               = 0;
volatile static uint8_t g_ext_temperature             = 0;
volatile static bool    g_temperature_updated         = false;
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
volatile static uint8_t  day;
volatile static uint8_t  month;
volatile static uint16_t year;
volatile static uint8_t  hours;
volatile static uint8_t  minutes;
volatile static uint8_t  seconds;
// ------------------------------------------------------------------------

volatile static uint32_t g_previous_timer_value        = 0;
volatile static bool     g_performance_times_available = false;
volatile static bool     g_sensors_updated             = true;
volatile static sensor_updates_t g_sensors_state       = {true, true, true};

//------------------------------------------------------------------------
//FXLS8974CF Accelerometer.
volatile static uint8_t INT_STATUS  = 0;
volatile static uint8_t TEMP_OUT    = 0;
volatile static uint8_t FXLS8974_TEMP = 0;
volatile static uint8_t LAST_FXLS8974_TEMP = 0;
volatile static uint16_t VECM    = 0;
volatile static uint16_t OUT_X   = 0;
volatile static uint16_t OUT_Y   = 0;
volatile static uint16_t OUT_Z   = 0;
volatile static float ACCEL_MAG  = 0.0;
volatile static float ACCEL_X    = 0.0;
volatile static float ACCEL_Y    = 0.0;
volatile static float ACCEL_Z    = 0.0;
volatile static float TEMP  = 0.0;

/* chart's variable (not affected by moving average) - integer version */
volatile static int16_t ACCEL_X_CHART    = 0;
volatile static int16_t ACCEL_Y_CHART    = 0;
volatile static int16_t ACCEL_Z_CHART    = 0;


//MPL3115 Pressure sensor variables
volatile static float MPL3115_PRESSURE_PA = 0;
volatile static float MPL3115_TEMP = 0.0;

volatile static uint16_t g_touch_sensitive            = 0;

//Moving average
uint8_t array_idx = 0;
float X_ACCEL_ARRAY [AVE_BUFFER_LENGHT] = {0};
float Y_ACCEL_ARRAY [AVE_BUFFER_LENGHT] = {0};
float Z_ACCEL_ARRAY [AVE_BUFFER_LENGHT] = {0};
float VMAG_ARRAY [AVE_BUFFER_LENGHT] = {0};
float TEMP_ARRAY [AVE_BUFFER_LENGHT] = {0};
//------------------------------------------------------

//Future implementation
void ADC0_IRQHandler(void)
{
    lpadc_conv_result_t convResultStruct;
    uint16_t Vbe1            = 0U;
    uint16_t Vbe8            = 0U;
    uint32_t convResultShift = 3U;
    float parameterSlope     = FSL_FEATURE_LPADC_TEMP_PARAMETER_A;
    float parameterOffset    = FSL_FEATURE_LPADC_TEMP_PARAMETER_B;
    float parameterAlpha     = FSL_FEATURE_LPADC_TEMP_PARAMETER_ALPHA;
    float temperature        = -273.15f; /* Absolute zero degree as the incorrect return value. */

	/* Read the 2 temperature sensor result. */
	if (true == LPADC_GetConvResult(ADC0, &convResultStruct, 0))
	{
		if(1 == convResultStruct.commandIdSource)
		{
			Vbe1 = convResultStruct.convValue >> convResultShift;
			if (true == LPADC_GetConvResult(ADC0, &convResultStruct, 0))
			{
				Vbe8 = convResultStruct.convValue >> convResultShift;
				/* Final temperature = A*[alpha*(Vbe8-Vbe1)/(Vbe8 + alpha*(Vbe8-Vbe1))] - B. */
				temperature = parameterSlope * (parameterAlpha * ((float)Vbe8 - (float)Vbe1) /
												((float)Vbe8 + parameterAlpha * ((float)Vbe8 - (float)Vbe1))) -
							  parameterOffset;
			}
			g_temperature_c = (temperature + g_temperature_c)/2;
			g_temperature_updated = true;
		}
		else if(2 == convResultStruct.commandIdSource)
		{
			g_touch_sensitive = convResultStruct.convValue;
		}
	}
}

/********************************************************************************************
	@brief : This function convert integer to string
	@param : value   -> Integer value to convert
	@param : string  -> Buffer pointer to save string number
	@param : int_len -> String length, if is more than number need is filled with '0'
	@return: Return sign of number (Not put '-' in string)
*********************************************************************************************/
sign_of_string_t int_to_string(int32_t value, uint8_t* string, uint8_t int_len)
{
	uint8_t counter;
	uint8_t index;
	uint8_t pow_cnt;
	uint32_t divisor;
	uint32_t suma = 0;
	sign_of_string_t sign = positive;

	if(value < 0)
	{
		sign = negative;
		value *= -1;
	}
	for(index = 0, counter = int_len; index < int_len; index ++, counter --)
	{
		divisor = 1;
		for(pow_cnt = 1; pow_cnt < counter; pow_cnt ++)
		{
			divisor *= 10;
		}
		string[index] = (value-suma)/divisor;
		suma += string[index]*divisor;
		string[index] += INT_TO_CHAR;
	}
	return sign;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function convert string to integer.
	@param : buffer -> String to convert, support negative number "-00050" or "-50" or "50"
	@param : size   -> Size of string
	@return: Integer number
*********************************************************************************************/
int32_t string_to_int(uint8_t buffer[], uint8_t size) // Ends when character is not a number or reach size
{
	uint8_t index;
	int32_t retval = 0;
	sign_of_string_t sign = positive;
	if(NULL != buffer)
	{
		for(index = 0; index < size && index < 100; index ++)
		{
			if('-' == buffer[index])
			{
				sign = negative;
			}
			else if('0' <= buffer[index] && '9' >= buffer[index])
			{
				retval *= 10;
				retval += (int8_t)buffer[index] + CHAR_TO_INT;
			}
			else
			{
				break;
			}
		}
		if(negative == sign)
		{
			retval *= -1;
		}
	}
	return retval;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function is for lvgl, if some value is updated this returns true
	@return: True if some value is updated, false if not
*********************************************************************************************/
bool sensor_updates_screen()
{
	bool sensors_updated = g_sensors_updated;
	g_sensors_updated = false;

	return sensors_updated;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This indicate values that are updated for lvgl (used for performance)
	@return: struct with flags that indicate updated values
*********************************************************************************************/
sensor_updates_t get_values_state_screen()
{
	sensor_updates_t sensor_updates = g_sensors_state;
	g_sensors_state.update_rtc             = false;
	g_sensors_state.update_mpl3115		   = false;
	g_sensors_state.update_fxls8974 	   = false;

	return sensor_updates;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of real time clock (RTC)
	@return: struct with all RTC values
*********************************************************************************************/
rtc_data_t get_rtc_data()
{
	rtc_data_t rtc_data;
	rtc_data.day     = day;
	rtc_data.month   = month;
	rtc_data.year    = year;
	rtc_data.hours   = hours;
	rtc_data.minutes = minutes;
	rtc_data.seconds = seconds;
	return rtc_data;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Periodic function used for update all accelerometer values, for "other" task in main
	@return: None
*********************************************************************************************/
bool update_accel()
{
	bool update = false;

	//Type and Initialize Variables Used by FXLS8974
	uint8_t rxBuff[SENS_DATA_LENGTH];  //data_length = 30

	//FXLS8974CF Sensor routine
	// Enable Vector Magnitude computation
	fxls8974_i2c_write(FXLS8974_SENS_CONFIG5, FXLS8974_SENS_CONFIG5_VECM_EN_MASK);

	//Include TEMP_OUT register when auto-incrementing on burst reads
	fxls8974_i2c_write(FXLS8974_SENS_CONFIG2, FXLS8974_SENS_CONFIG2_ANIC_TEMP_EN);
	fxls8974_i2c_read(FXLS8974_SENS_CONFIG2, rxBuff, 1);

	// Set ±2g Range and Set ACTIVE
	fxls8974_i2c_write(FXLS8974_SENS_CONFIG1,
			(FXLS8974_SENS_CONFIG1_FSR_2G | FXLS8974_SENS_CONFIG1_ACTIVE_ACTIVE));

	// Set ODR = 3200 Hz
	fxls8974_i2c_write(FXLS8974_SENS_CONFIG3,
				(FXLS8974_SENS_CONFIG3_WAKE_ODR_3200HZ | FXLS8974_SENS_CONFIG3_SLEEP_ODR_3200HZ));

	/* Wait until the slave is ready for transmit, wait time depend on user's case.
	   Slave devices that need some time to process received byte or are not ready yet to
	   send the next byte, can pull the clock low to signal to the master that it should wait.*/
	//vTaskDelay(20);

	/* Burst I2C Read from register 00x00 */
	memset(rxBuff, 0, sizeof(rxBuff)); 	/* rxBuff will contain 10 bytes with the last sensor data */
	if (fxls8974_i2c_read(FXLS8974_START_BURST_ADDR, rxBuff, FXLS8974_READ_10BYTES) != kStatus_Success)
	{
		PRINTF("Error when reading Sensor\r\n");
	}
	else {
		//PRINTF("Sensor data read successfully\r\n");
		;
	}

	// Issue a Soft Reset (Clears auto-increment counters for next Burst Read)
	fxls8974_i2c_write(FXLS8974_SENS_CONFIG1, FXLS8974_SENS_CONFIG1_RST_MASK);

	INT_STATUS = rxBuff[0];
	TEMP_OUT   = rxBuff[1];

	//data decoding
	VECM  = (rxBuff[3]<<8) + rxBuff[2];  //assemble Bytes to 16 bit UINTs
	OUT_X = (rxBuff[5]<<8) + rxBuff[4];
	OUT_Y = (rxBuff[7]<<8) + rxBuff[6];
	OUT_Z = (rxBuff[9]<<8) + rxBuff[8];

	//Convert to units of degC
	//Interpret as 2's compliment
	if (TEMP_OUT > 127) {
	  TEMP_OUT = ~TEMP_OUT + 1;
	  FXLS8974_TEMP = 25 - TEMP_OUT;
	}
	else {
	  FXLS8974_TEMP = TEMP_OUT + 25;
	}

	//Convert to units of g
	//Interpret as 2's compliment
	ACCEL_X = get_accel_mg(OUT_X);
	ACCEL_Y = get_accel_mg(OUT_Y);
	ACCEL_Z = get_accel_mg(OUT_Z);
	ACCEL_MAG = get_accel_mg(VECM);  /*Vector magnitude is an integrated
									   feature of the sensor FXLS8974CF
									   (distance of from the origin) */

	/* These variables change faster than ones passed through mov.ave. filter */
	ACCEL_X_CHART = ACCEL_X;
	ACCEL_Y_CHART = ACCEL_Y;
	ACCEL_Z_CHART = ACCEL_Z;

	/* MOVING_AVERAGE computation for magnitude indicator on display */
#ifdef MOVING_AVERAGE_ENABLE
	//Fill in ACCEL_X array
	X_ACCEL_ARRAY[array_idx] = ACCEL_X;
	ACCEL_X = Moving_Average(X_ACCEL_ARRAY, ACCEL_X);

	//Fill in ACCEL_Y array
	Y_ACCEL_ARRAY[array_idx] = ACCEL_Y;
	ACCEL_Y = Moving_Average(Y_ACCEL_ARRAY,ACCEL_Y);

	//Fill in ACCEL_Z array
	Z_ACCEL_ARRAY[array_idx] = ACCEL_Z;
	ACCEL_Z = Moving_Average(Z_ACCEL_ARRAY, ACCEL_Z);

	//Fill in ACCEL_MAG array
	VMAG_ARRAY[array_idx] = ACCEL_MAG;
	ACCEL_MAG = Moving_Average(VMAG_ARRAY, ACCEL_MAG);

	//Fill in TEMPERATURE array
	TEMP_ARRAY[array_idx] = (float)FXLS8974_TEMP;
	FXLS8974_TEMP = (uint8_t)Moving_Average(TEMP_ARRAY, FXLS8974_TEMP);

	array_idx++;

	if (array_idx == AVE_BUFFER_LENGHT) /* Restart pointer to arrays for mov.average */
	  array_idx = 0;

#endif

	//Print data to terminal
	PRINTF("\r\n\r\n>>> FXLS8974:");
	PRINTF(" %d degC,", FXLS8974_TEMP);
	PRINTF(" %.2f mGX, %.2f mGY, %.2f mGZ, %.2f mGV", ACCEL_X, ACCEL_Y, ACCEL_Z, ACCEL_MAG);

	update = true; /* There was an update of the pressure values */
	g_sensors_state.update_fxls8974 = true;

	return update;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Periodic function used for update all pressure values, for "other" task in main
	@return: None
*********************************************************************************************/
bool update_pressure()
{
	bool update      = false;
	uint8_t rxBuff[SENS_DATA_LENGTH];  //data_length = 30

	//NXP MPL3115
	// Place the part in ACTIVE mode, using 4:1 Oversampling
	mpl3115_i2c_write(MPL3115_CTRL_REG1, (MPL3115_CTRL_REG1_OS_OSR_4 | MPL3115_CTRL_REG1_SBYB_ACTIVE));
	//mpl3115_i2c_read(MPL3115_WHO_AM_I, rxBuff, 2);

	vTaskDelay(20);

	/* Burst I2C Read from register 00x00 */
	memset(rxBuff, 0, sizeof(rxBuff)); 	/* Clean rxBuff before reading 6 bytes last sensor data */
	if (mpl3115_i2c_read(MPL3115_START_BURST_ADDR, rxBuff, MPL3115_READ_6BYTES) != kStatus_Success)
	{
		PRINTF("\r\nError when reading Sensor\n\r");
	}
	else {
		;
	}

	// Place the MPL3115 sensor in STANDBY mode
	mpl3115_i2c_write(MPL3115_CTRL_REG1, MPL3115_CTRL_REG1_SBYB_STANDBY);

	//convert from unsigned Q18.2 format to Float
	MPL3115_PRESSURE_PA = (rxBuff[1] << 10) + (rxBuff[2] << 2) + ((rxBuff[3] & 0xC0) >> 6); //
	if (rxBuff[3] & 0x20) {MPL3115_PRESSURE_PA += 0.5000;}    //rxBuff is OUT_P_LSB
	if (rxBuff[3] & 0x10) {MPL3115_PRESSURE_PA += 0.2500;}

	//convert from signed Q8.4 format to Float
	MPL3115_TEMP = rxBuff[4];
	if (rxBuff[5] & 0x80) {MPL3115_TEMP += 0.5000;}
	if (rxBuff[5] & 0x40) {MPL3115_TEMP += 0.2500;}
	if (rxBuff[5] & 0x20) {MPL3115_TEMP += 0.1250;}
	if (rxBuff[5] & 0x10) {MPL3115_TEMP += 0.0625;}

	PRINTF("\r\n>>> MPL3115:");
	PRINTF(" %d degC, ", (int16_t)MPL3115_TEMP);
	PRINTF(" %d Pascals.", (uint32_t)MPL3115_PRESSURE_PA);

	g_sensors_state.update_mpl3115 = true;
	update = true; /* TODO: To implement check */

	return update;

}

/********************************************************************************************
	@brief : Periodic function used for update all values, called for "other" task in main
	@return: None
*********************************************************************************************/
void update_values()
{
	bool updated = false;
	if((false == g_sensors_updated))
	{
		updated |= update_accel();
		updated |= update_pressure();

		g_sensors_updated |= updated;
	}
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get accelerometer values (mikroe board)
	@return: struct with both accelerometer values
*********************************************************************************************/
fxls8974_data_t get_accel_data()
{
	fxls8974_data_t fxls8974_sensor;

	fxls8974_sensor.x_axis_mg = (int16_t)ACCEL_X;
	fxls8974_sensor.y_axis_mg = (int16_t)ACCEL_Y;
	fxls8974_sensor.z_axis_mg = (int16_t)ACCEL_Z;
	fxls8974_sensor.v_mag_mg = (int16_t)ACCEL_MAG;
	fxls8974_sensor.temperature = FXLS8974_TEMP;
	fxls8974_sensor.status = INT_STATUS;

	return fxls8974_sensor;

}

mpl3115_data_t get_pressure_data(void)
{
	mpl3115_data_t mpl3115_sensor;

	mpl3115_sensor.pressure_pa = MPL3115_PRESSURE_PA;
	mpl3115_sensor.temperature = MPL3115_TEMP;
	mpl3115_sensor.status = 0; /* Not yet implemented */

	return mpl3115_sensor;
}

fxls8974_chart_data_t get_chart_data(void)
{
	fxls8974_chart_data_t chart_data;

	chart_data.x_axis_chart = ACCEL_X_CHART;
	chart_data.y_axis_chart = ACCEL_Y_CHART;
	chart_data.z_axis_chart = ACCEL_Z_CHART;

	return chart_data;
}


float get_accel_mg(uint16_t accel_read)
{
  /* Convert to units of g */
  //Interpret as 2's compliment, 12-bit number
  float accel;
  float sensitivity = 0.98;  // ±2g range

  if (accel_read > 2047) {
    accel_read = ~accel_read + 1;
    accel = (-sensitivity) * accel_read;
  }

  else
  {
    accel = sensitivity * accel_read;
    }

  return accel;
}

float Moving_Average(float* X_ACCEL_ARRAY, float ACCEL_X)
{
  /* Moving average for Accelerometer data */
  float cum = 0;

  for (uint8_t i=0; i< AVE_BUFFER_LENGHT; i++){
    cum += X_ACCEL_ARRAY[i];
  }

  return cum / AVE_BUFFER_LENGHT;
}

