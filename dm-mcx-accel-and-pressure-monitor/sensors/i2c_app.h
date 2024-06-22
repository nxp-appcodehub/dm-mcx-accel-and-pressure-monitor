/*
 * Copyright  2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef I2C_APP_H_
#define I2C_APP_H_

#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_MASTER_BASE    (LPI2C3_BASE)
#define I2C_MASTER_CLOCK_FREQUENCY CLOCK_GetMainClkFreq()
#define WAIT_TIME                  100U
#define EXAMPLE_I2C_MASTER ((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)

#define FXLS8974_I2C_SLAVE_ADDR_7BIT 0x18U  /* 0x18 when SA0 = 0, 0x19 when SA0 = 1 */
#define MPL3115_I2C_SLAVE_ADDR_7BIT  0x60U
#define I2C_BAUDRATE               100000U
#define SENS_DATA_LENGTH            30U

#define FXLS8974_READ_10BYTES    10U
#define MPL3115_READ_6BYTES      6U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
//fxls8974
status_t fxls8974_i2c_read(uint8_t register_addr, uint8_t * rxBuff_Sens, uint8_t byte_number);
status_t fxls8974_i2c_transfer_write(uint8_t register_addr, uint8_t data);
uint8_t FXLS8974_init(void);
status_t fxls8974_i2c_write(uint8_t register_addr, uint8_t data);
status_t fxls8974_i2c_transfer_read(uint8_t register_addr, uint8_t * g_master_rxBuff, uint8_t byte_number);

//mpl3115
status_t mpl3115_i2c_read(uint8_t register_addr, uint8_t * rxBuff_Sens, uint8_t byte_number);
status_t mpl3115_i2c_write(uint8_t register_addr, uint8_t data);
uint8_t MPL3115_init(void);


/*******************************************************************************
 * Variables
 ******************************************************************************/
extern uint8_t g_master_txBuff[SENS_DATA_LENGTH];
extern uint8_t g_master_rxBuff[SENS_DATA_LENGTH];

extern volatile bool g_MasterCompletionFlag;
extern lpi2c_master_handle_t g_m_handle;

#endif /* I2C_APP_H_ */
