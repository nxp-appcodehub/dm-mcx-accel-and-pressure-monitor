/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TIME_FUNC_H_
#define TIME_FUNC_H_

#include <stdint.h>

extern void SysTick_DelayTicks(uint32_t n);

void delay(uint16_t time_ms);

#endif /* TIME_FUNC_H_ */
