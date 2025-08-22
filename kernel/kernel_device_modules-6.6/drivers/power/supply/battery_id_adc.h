/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020.
 */
#ifndef __BATTERY_ID_ADC__
#define __BATTERY_ID_ADC__

#define    BATTERY_SDI_ID_VOLT_HIGH            627
#define    BATTERY_SDI_ID_VOLT_LOW             513
#define    BATTERY_BYD_ID_VOLT_HIGH            1320
#define    BATTERY_BYD_ID_VOLT_LOW             1080
#define    BATTERY_ATL_ID_VOLT_HIGH            990
#define    BATTERY_ATL_ID_VOLT_LOW             810
#define    BATTERY_GF_ID_VOLT_HIGH            400
#define    BATTERY_GF_ID_VOLT_LOW             250


int bat_id_get_adc_num(void);
signed int battery_get_bat_id_voltage(void);
int battery_get_profile_id(void);

#endif