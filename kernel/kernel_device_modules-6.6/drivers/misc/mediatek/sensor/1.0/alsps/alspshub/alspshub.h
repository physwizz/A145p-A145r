/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

/*
 * Definitions for ALSPS als/ps sensor chip.
 */
#ifndef __ALSPSHUB_H__
#define __ALSPSHUB_H__

#include <linux/ioctl.h>


/*ALSPS related driver tag macro*/
#define ALSPS_SUCCESS						0
#define ALSPS_ERR_I2C						-1
#define ALSPS_ERR_STATUS					-3
#define ALSPS_ERR_SETUP_FAILURE				-4
#define ALSPS_ERR_GETGSENSORDATA			-5
#define ALSPS_ERR_IDENTIFICATION			-6

/*----------------------------------------------------------------------------*/
enum ALSPS_NOTIFY_TYPE {
	ALSPS_NOTIFY_PROXIMITY_CHANGE = 0,
};

/*a06 code for SR-AL7160V-01-19|SR-AL6528V-01-177 by zhawei at 20240826 start*/
/*a06 code for AL7160AV-17 by liuyiying at 20240910 start*/
#if defined(CONFIG_HQ_PROJECT_O22)
enum lcd_id {
	LCD_NONE,
	LCD_FIRST,
	LCD_SECOND,
	LCD_THIRD,
	LCD_FOURTH,
};
#endif    //CONFIG_HQ_PROJECT_O22

#if defined(CONFIG_HQ_PROJECT_O8)
enum lcd_id {
	LCD_NONE,
	LCD_FIRST,
	LCD_SECOND,
	LCD_THIRD,
	LCD_FOURTH,
	LCD_FIFTH,
	LCD_SIXTH,
	LCD_SEVENTH,
};
#endif    //CONFIG_HQ_PROJECT_O8

#if defined(CONFIG_HQ_PROJECT_O22) || defined(CONFIG_HQ_PROJECT_O8)
struct lcd_id_info
{
	enum lcd_id hwid;
	char *lcd_strdata;
};
#endif    //CONFIG_HQ_PROJECT_O22 || CONFIG_HQ_PROJECT_O8

#if defined(CONFIG_HQ_PROJECT_O22)
struct lcd_id_info lcd_info[] = {
	{LCD_FIRST,  "hwid:0x13"},
	{LCD_SECOND, "hwid:0x23"},
	{LCD_THIRD,  "hwid:0x33"},
	{LCD_FOURTH, "hwid:0x43"},
};
#endif    //CONFIG_HQ_PROJECT_O22

#if defined(CONFIG_HQ_PROJECT_O8)
struct lcd_id_info lcd_info[] = {
	{LCD_FIRST,  "hwid:0x30"},
	{LCD_SECOND, "hwid:0x20"},
	{LCD_THIRD,  "hwid:0x50"},
	{LCD_FOURTH, "hwid:0x60"},
	{LCD_FIFTH,  "hwid:0x70"},
	{LCD_SIXTH,  "hwid:0x80"},
	{LCD_SEVENTH,"hwid:0x90"}
};
#endif    //CONFIG_HQ_PROJECT_O8
/*a06 code for AL7160AV-17 by liuyiying at 20240910 end*/
/*a06 code for SR-AL7160V-01-19|SR-AL6528V-01-177 by zhawei at 20240826 end*/

#endif
