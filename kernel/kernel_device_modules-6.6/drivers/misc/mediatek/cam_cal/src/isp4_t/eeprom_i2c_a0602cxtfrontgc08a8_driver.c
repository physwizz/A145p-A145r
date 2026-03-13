/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */
#define PFX "CAM_CAL"
#define pr_fmt(fmt) PFX "[%s] " fmt, __func__


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/of.h>
#include "cam_cal.h"
#include "cam_cal_define.h"
#include "cam_cal_list.h"
#include <linux/dma-mapping.h>
#ifdef CONFIG_COMPAT
/* 64 bit */
#include <linux/fs.h>
#include <linux/compat.h>
#endif

#include "eeprom_i2c_a0602cxtfrontgc08a8_driver.h"

static struct i2c_client *g_pstI2CclientG;

#define GC08A3_OTP_DEBUG  1
#define GC08A3_I2C_ID     0x62 /*0x20*/

// A06 code for SR-AL7160A by xuyunhui at 20240902 start
static int GC08A8_Otp_Read_I2C_CAM_CAL(u16 a_u2Addr)
{
	int i4RetValue = 0;
	u16 data;
	char puReadCmd[2] = { (char)(a_u2Addr >> 8), (char)(a_u2Addr & 0xFF) };
	struct i2c_msg msg[2];
    u8 a_puBuff[1];
	msg[0].addr = g_pstI2CclientG->addr;
	msg[0].flags = g_pstI2CclientG->flags & I2C_M_TEN;
	msg[0].len = 2;
	msg[0].buf = puReadCmd;

	msg[1].addr = g_pstI2CclientG->addr;
	msg[1].flags = g_pstI2CclientG->flags & I2C_M_TEN;
	msg[1].flags |= I2C_M_RD;
	msg[1].len = 1;
	msg[1].buf = a_puBuff;

	i4RetValue = i2c_transfer(g_pstI2CclientG->adapter, msg, 2);

	if (i4RetValue != 2) {
		printk("GC08A8 I2C read data failed!!\n");
		return -1;
	}
	data = a_puBuff[0];
	return data;
}

static int GC08A8_Otp_Write_I2C_CAM_CAL_U8(u16 a_u2Addr,u8 parameter)
{
	int i4RetValue = 0;
	char puCmd[3];
	struct i2c_msg msg;
	puCmd[0] = (char)(a_u2Addr >> 8);
	puCmd[1] = (char)(a_u2Addr & 0xFF);
	puCmd[2] = (char)(parameter & 0xFF);
	msg.addr = g_pstI2CclientG->addr;
	msg.flags = g_pstI2CclientG->flags & I2C_M_TEN;
	msg.len = 3;
	msg.buf = puCmd;

	i4RetValue = i2c_transfer(g_pstI2CclientG->adapter, &msg, 1);

	if (i4RetValue != 1) {
		printk("GC08A8 I2C write data failed!!\n");
		return -1;
	}
	mdelay(5);
	return 0;
}

static u16 read_cmos_sensor(u16 addr)
{
	u16 get_byte=0;
        get_byte = GC08A8_Otp_Read_I2C_CAM_CAL(addr);

	return get_byte;
}

static void write_cmos_sensor(u16 addr, u8 para)
{
    GC08A8_Otp_Write_I2C_CAM_CAL_U8(addr, para);
}
// A06 code for SR-AL7160A by xuyunhui at 20240902 end

static void gc08a3_otp_init(void)
{
	write_cmos_sensor(0x0315, 0x80);
	write_cmos_sensor(0x031c, 0x60);

	write_cmos_sensor(0x0324, 0x42);
	write_cmos_sensor(0x0316, 0x09);
	write_cmos_sensor(0x0a67, 0x80);
	write_cmos_sensor(0x0313, 0x00);
	write_cmos_sensor(0x0a53, 0x0e);
	write_cmos_sensor(0x0a65, 0x17);
	write_cmos_sensor(0x0a68, 0xa1);
	write_cmos_sensor(0x0a47, 0x00);
	write_cmos_sensor(0x0a58, 0x00);
	write_cmos_sensor(0x0ace, 0x0c);
}

static void gc08a3_otp_close(void)
{
	write_cmos_sensor(0x0316, 0x01);
	write_cmos_sensor(0x0a67, 0x00);
}

static u16 gc08a3_otp_read_group(u16 addr, u8 *data, u16 length)
{
	u16 i = 0;

	write_cmos_sensor(0x0313, 0x00);
	write_cmos_sensor(0x0a69, (addr >> 8) & 0xff);
	write_cmos_sensor(0x0a6a, addr & 0xff);
	write_cmos_sensor(0x0313, 0x20);
	write_cmos_sensor(0x0313, 0x12);

	for (i = 0; i < length; i++) {
		data[i] = read_cmos_sensor(0x0a6c);
#if GC08A3_OTP_DEBUG
	pr_debug("addr = 0x%x, data = 0x%x\n", addr + i * 8, data[i]);
#endif
	}
	return 0;
}


int gc08a3_iReadData(unsigned int ui4_offset,
	unsigned int ui4_length, unsigned char *pinputdata)
{
	int i4RetValue = 0;

	pr_debug("gc08a3 otp ui4_offset = 0x%x, ui4_length = %d \n", ui4_offset, ui4_length);

	gc08a3_otp_init();
	mdelay(10);

	i4RetValue = gc08a3_otp_read_group(ui4_offset, pinputdata, ui4_length);
	if (i4RetValue != 0) {
		pr_debug("I2C iReadData failed!!\n");
		return -1;
	}

	gc08a3_otp_close();

	return 0;
}

unsigned int a0602cxtfrontgc08a8_read_region(struct i2c_client *client, unsigned int addr,
				unsigned char *data, unsigned int size)
{
	g_pstI2CclientG = client;
	if (gc08a3_iReadData(addr, size, data) == 0)
		return size;
	else
		return 0;
}
