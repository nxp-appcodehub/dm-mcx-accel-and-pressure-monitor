/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "time_func.h"


void delay(uint16_t time_ms)
{

	SysTick_DelayTicks(time_ms);
}
