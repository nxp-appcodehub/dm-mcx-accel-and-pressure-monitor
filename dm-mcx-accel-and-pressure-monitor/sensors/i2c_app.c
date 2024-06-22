/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  Standard C Included Files */
#include <i2c_app.h>
#include "fxls8974.h"
#include "mpl3115.h"


/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t g_master_txBuff[SENS_DATA_LENGTH];
uint8_t g_master_rxBuff[SENS_DATA_LENGTH];

//I2C global variables
volatile bool g_MasterCompletionFlag = false;
lpi2c_master_handle_t g_m_handle;

status_t fxls8974_i2c_write(uint8_t register_addr, uint8_t data)
{
    status_t reVal        = kStatus_Fail;

	g_master_txBuff[0] = register_addr;
	g_master_txBuff[1] = data;

    /* Send master blocking data to slave */
    if (kStatus_Success == LPI2C_MasterStart(EXAMPLE_I2C_MASTER, FXLS8974_I2C_SLAVE_ADDR_7BIT, kLPI2C_Write))
    {
        reVal = LPI2C_MasterSend(EXAMPLE_I2C_MASTER, g_master_txBuff, 2);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterStop(EXAMPLE_I2C_MASTER);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }
    }

    return reVal;
}

status_t fxls8974_i2c_transfer_write(uint8_t register_addr, uint8_t data)
{
	lpi2c_master_transfer_t masterXfer = {0};
    status_t reVal        = kStatus_Fail;

	g_master_txBuff[0] = register_addr;
	g_master_txBuff[1] = data;

    /* uint8_t deviceAddress     = 0x01U; */
    masterXfer.slaveAddress   = FXLS8974_I2C_SLAVE_ADDR_7BIT;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.subaddress     = (uint32_t)0;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = g_master_txBuff;
    masterXfer.dataSize       = 2;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    /* Send master non-blocking data to slave */
    reVal = LPI2C_MasterTransferNonBlocking(EXAMPLE_I2C_MASTER, &g_m_handle, &masterXfer);

    /*  Reset master completion flag to false. */
    g_MasterCompletionFlag = false;

    if (reVal != kStatus_Success)
    {
        return -1;
    }

    /*  Wait for transfer completed. */
    while (!g_MasterCompletionFlag) /* it becomes true when ISR is called. */
    {
    }
    g_MasterCompletionFlag = false;

    return reVal;

}

status_t fxls8974_i2c_transfer_read(uint8_t register_addr, uint8_t * g_master_rxBuff, uint8_t byte_number)
{
	status_t reVal = kStatus_Fail;
	lpi2c_master_transfer_t masterXfer = {0};

    // Read I2C data from slave
	g_master_rxBuff[0] = register_addr;

    /* uint8_t deviceAddress     = 0x01U; */
    masterXfer.slaveAddress   = FXLS8974_I2C_SLAVE_ADDR_7BIT;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.subaddress     = (uint32_t)0;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = g_master_rxBuff;
    masterXfer.dataSize       = byte_number;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    /* Send master non-blocking data to slave */
    reVal = LPI2C_MasterTransferNonBlocking(EXAMPLE_I2C_MASTER, &g_m_handle, &masterXfer);

    /*  Reset master completion flag to false. */
    g_MasterCompletionFlag = false;

    if (reVal != kStatus_Success)
    {
        return -1;
    }

    /*  Wait for transfer completed. */
    while (!g_MasterCompletionFlag) /* it becomes true when ISR is called. */
    {
    }
    g_MasterCompletionFlag = false;

    return reVal;

}


status_t fxls8974_i2c_read(uint8_t register_addr, uint8_t * g_master_rxBuff, uint8_t byte_number)
{
    status_t reVal = kStatus_Fail;

	// Read I2C data from slave
	g_master_txBuff[0] = register_addr;

	/* Receive blocking data from slave */
	if (kStatus_Success == LPI2C_MasterStart(EXAMPLE_I2C_MASTER, FXLS8974_I2C_SLAVE_ADDR_7BIT, kLPI2C_Write))
	{

		// 0 set as the starting register for a Burst Read
		reVal = LPI2C_MasterSend(EXAMPLE_I2C_MASTER, g_master_txBuff, 1);
		if (reVal != kStatus_Success)
		{
			return -1;
		}

		reVal = LPI2C_MasterRepeatedStart(EXAMPLE_I2C_MASTER, FXLS8974_I2C_SLAVE_ADDR_7BIT, kLPI2C_Read);
		if (reVal != kStatus_Success)
		{
			return -1;
		}

		reVal =
				LPI2C_MasterReceive(EXAMPLE_I2C_MASTER, g_master_rxBuff, byte_number);
		if (reVal != kStatus_Success)
		{
			return -1;
		}

		reVal = LPI2C_MasterStop(EXAMPLE_I2C_MASTER);
		if (reVal != kStatus_Success)
		{
			return -1;
		}
	}

	return reVal;

}

/* MPL3115 Functions */
status_t mpl3115_i2c_write(uint8_t register_addr, uint8_t data)
{
    status_t reVal        = kStatus_Fail;

	g_master_txBuff[0] = register_addr;
	g_master_txBuff[1] = data;

    /* Send master blocking data to slave */
    if (kStatus_Success == LPI2C_MasterStart(EXAMPLE_I2C_MASTER, MPL3115_I2C_SLAVE_ADDR_7BIT, kLPI2C_Write))
    {
        //Wire.write(0x19);         // Write to SENSE_CONFIG5
        //Wire.write(0x10);         // Enable the calculation of Vector Magnitude (disabled by default)
        reVal = LPI2C_MasterSend(EXAMPLE_I2C_MASTER, g_master_txBuff, 2);
        if (reVal != kStatus_Success)
        {
            return -1;
        }

        //Wire.endTransmission(1);  // 1 sends a STOP and releases the I2C bus
        reVal = LPI2C_MasterStop(EXAMPLE_I2C_MASTER);
        if (reVal != kStatus_Success)
        {
            return -1;
        }
    }

    return reVal;

}


status_t mpl3115_i2c_read(uint8_t register_addr, uint8_t * g_master_rxBuff, uint8_t byte_number)
{
    status_t reVal = kStatus_Fail;

	// Read I2C data from slave
	g_master_txBuff[0] = register_addr;

	/* Receive blocking data from slave */
	if (kStatus_Success == LPI2C_MasterStart(EXAMPLE_I2C_MASTER, MPL3115_I2C_SLAVE_ADDR_7BIT, kLPI2C_Write))
	{

		// 0 set as the starting register for a Burst Read
		reVal = LPI2C_MasterSend(EXAMPLE_I2C_MASTER, g_master_txBuff, 1);
		if (reVal != kStatus_Success)
		{
			return -1;
		}

		reVal = LPI2C_MasterRepeatedStart(EXAMPLE_I2C_MASTER, MPL3115_I2C_SLAVE_ADDR_7BIT, kLPI2C_Read);
		if (reVal != kStatus_Success)
		{
			return -1;
		}

		reVal =
				LPI2C_MasterReceive(EXAMPLE_I2C_MASTER, g_master_rxBuff, byte_number);
		if (reVal != kStatus_Success)
		{
			return -1;
		}

		reVal = LPI2C_MasterStop(EXAMPLE_I2C_MASTER);
		if (reVal != kStatus_Success)
		{
			return -1;
		}
	}

	return reVal;

}

uint8_t FXLS8974_Get_WHO_AM_I_Reg(uint8_t *who_am_i)
{
	uint8_t status;
	uint8_t rxBuff[4];

	// Read WHO_AM_I register from FXLS8974CF
	if (fxls8974_i2c_read(FXLS8974_WHO_AM_I, rxBuff, 1) == kStatus_Success)
	{
		status = devStatus_Success;
		*who_am_i = rxBuff[0]; //read first byte of buffer.
	}
	else
		status = devStatus_Fail;

	return status;
}

uint8_t MPL3115_Get_WHO_AM_I_Reg(uint8_t *who_am_i)
{
	uint8_t status;
	uint8_t rxBuff[4];

	// Read WHO_AM_I register from MPL3115
	if (mpl3115_i2c_read(MPL3115_WHO_AM_I, rxBuff, 1) == kStatus_Success)
	{
		status = devStatus_Success;
		*who_am_i = rxBuff[0]; //read first byte of buffer.
	}
	else
		status = devStatus_Fail;

	return status;
}

uint8_t FXLS8974_init (void)
{
	uint8_t fxls8974_who_am_i;
	uint8_t status;

	status = FXLS8974_Get_WHO_AM_I_Reg(&fxls8974_who_am_i); /* Read FXLS8974 WHO_AM_I register. */

#if ENABLE_RETRY
		uint8_t retry_cnt = 0;
		while (retry_cnt < RETRY_COUNT && status != devStatus_Success)
		{
			// Retry communicating with target device after resetting it
			// ResetTarget();

			status = FXLS8974_Get_WHO_AM_I_Reg(&fxls8974_who_am_i);
			retry_cnt++;
		}
#endif

	if (status == devStatus_Success)
	{
		if (fxls8974_who_am_i == 0x86)
		{
			PRINTF("\r\nSensor FXLS8974 is present on the board\r\n");
		}
		else
		{
			PRINTF("Sensor address received: %d.", fxls8974_who_am_i);
		}
	}

	return status;
}

uint8_t MPL3115_init(void)
{
	uint8_t status;

	/* Read MPL3115 WHO_AM_I register. */
	uint8_t mpl3115_who_am_i;
	status = MPL3115_Get_WHO_AM_I_Reg(&mpl3115_who_am_i);

#if ENABLE_RETRY
		uint8_t retry_cnt = 0;
		while (retry_cnt < RETRY_COUNT && status != devStatus_Success)
		{
			// Retry communicating with target device after resetting it
			// ResetTarget();

			status = MPL3115_Get_WHO_AM_I_Reg(&mpl3115_who_am_i);
			retry_cnt++;
		}
#endif

	if (status == devStatus_Success)
	{
		if ((mpl3115_who_am_i == 0xC4) || (mpl3115_who_am_i == 0xC5))
		{
			PRINTF("\r\nSensor MPL3115 is present on the board\r\n");
		}
		else
		{
			PRINTF("Sensor address received: %d.", mpl3115_who_am_i);
		}
	}

	return status;
}
