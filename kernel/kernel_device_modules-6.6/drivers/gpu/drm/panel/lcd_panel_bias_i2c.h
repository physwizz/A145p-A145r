/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef _LCD_PANEL_BIAS_I2C_H_
#define _LCD_PANEL_BIAS_I2C_H_

#include <linux/list.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio/consumer.h>
#include <linux/pinctrl/consumer.h>
#include <linux/delay.h>



int lcd_panel_i2c_write_bytes(unsigned char addr, unsigned char value);
int lcd_panel_i2c_read_bytes(unsigned char addr, unsigned char *value);

/* function: write i2c data buffer
 * input: addr: i2c address,
 *		value: data buffer
 *		size: data size
 * output: 0 for success; !0 for failed
 */
int lcd_panel_i2c_write_multiple_bytes(unsigned char addr, unsigned char *value,
		unsigned int size);

#endif//_LCD_PANEL_BIAS_I2C_H_
