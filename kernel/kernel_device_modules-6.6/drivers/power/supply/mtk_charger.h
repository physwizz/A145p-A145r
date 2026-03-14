/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_CHARGER_H
#define __MTK_CHARGER_H

#include <linux/alarmtimer.h>
#include "charger_class.h"
#include "adapter_class.h"
#include "mtk_charger_algorithm_class.h"
#include <linux/power_supply.h>
#include "mtk_smartcharging.h"

#define CHARGING_INTERVAL 10
#define CHARGING_FULL_INTERVAL 20

#define CHRLOG_ERROR_LEVEL	1
#define CHRLOG_INFO_LEVEL	2
#define CHRLOG_DEBUG_LEVEL	3

#define SC_TAG "smartcharging"

extern int chr_get_debug_level(void);

#define chr_err(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_ERROR_LEVEL) {	\
		pr_notice(fmt, ##args);				\
	}							\
} while (0)

#define chr_info(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_INFO_LEVEL) {	\
		pr_notice_ratelimited(fmt, ##args);		\
	}							\
} while (0)

#define chr_debug(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_DEBUG_LEVEL) {	\
		pr_notice(fmt, ##args);				\
	}							\
} while (0)

struct mtk_charger;
struct charger_data;
#define BATTERY_CV 4350000
#define V_CHARGER_MAX 6500000 /* 6.5 V */
#define V_CHARGER_MIN 4600000 /* 4.6 V */
#define VBUS_OVP_VOLTAGE 15000000 /* 15V */
/* dual battery */
#define V_CS_BATTERY_CV 4350 /* mV */
#define AC_CS_NORMAL_CC 2000 /* mV */
#define AC_CS_FAST_CC 2000 /* mV */
#define CS_CC_MIN 100 /* mA */
#define V_BATT_EXTRA_DIFF 300 /* 265 mV */

#define USB_CHARGER_CURRENT_SUSPEND		0 /* def CONFIG_USB_IF */
#define USB_CHARGER_CURRENT_UNCONFIGURED	70000 /* 70mA */
#define USB_CHARGER_CURRENT_CONFIGURED		500000 /* 500mA */
#define USB_CHARGER_CURRENT			500000 /* 500mA */
#define AC_CHARGER_CURRENT			2050000
#define AC_CHARGER_INPUT_CURRENT		3200000
#define NON_STD_AC_CHARGER_CURRENT		500000
#define CHARGING_HOST_CHARGER_CURRENT		650000

/* dynamic mivr */
#define V_CHARGER_MIN_1 4400000 /* 4.4 V */
#define V_CHARGER_MIN_2 4200000 /* 4.2 V */
#define MAX_DMIVR_CHARGER_CURRENT 1800000 /* 1.8 A */

/* battery warning */
#define BATTERY_NOTIFY_CASE_0001_VCHARGER
#define BATTERY_NOTIFY_CASE_0002_VBATTEMP

/* charging abnormal status */
#define CHG_VBUS_OV_STATUS	(1 << 0)
#define CHG_BAT_OT_STATUS	(1 << 1)
#define CHG_OC_STATUS		(1 << 2)
#define CHG_BAT_OV_STATUS	(1 << 3)
#define CHG_ST_TMO_STATUS	(1 << 4)
#define CHG_BAT_LT_STATUS	(1 << 5)
#define CHG_TYPEC_WD_STATUS	(1 << 6)
#define CHG_DPDM_OV_STATUS	(1 << 7)

/* Battery Temperature Protection */
#define MIN_CHARGE_TEMP  0
#define MIN_CHARGE_TEMP_PLUS_X_DEGREE	6
#define MAX_CHARGE_TEMP  50
#define MAX_CHARGE_TEMP_MINUS_X_DEGREE	47

#define MAX_ALG_NO 10
/*A06_V code for SR-AL7160V-01-163 by xiongxiaoliang at 20240912 start*/
/*A14 code for SR-AL6528V-01-26 by zhangziyi at 20240929 start*/
#if IS_ENABLED(CONFIG_HQ_PROJECT_O8) || IS_ENABLED(CONFIG_HQ_PROJECT_O22)
#define MAX_CV_ENTRIES	8
#define MAX_CYCLE_COUNT	0xFFFF
#define is_between(left, right, value) \
	(((left) >= (right) && (left) >= (value) && \
	(value) >= (right)) || \
	((left) <= (right) && (left) <= (value) && \
	(value) <= (right)))

struct range_data {
	u32 low_threshold;
	u32 high_threshold;
	u32 value;
};
#endif
/*A14 code for SR-AL6528V-01-26 by zhangziyi at 20240929 end*/
/*A06_V code for SR-AL7160V-01-163 by xiongxiaoliang at 20240912 end*/
#define RESET_BOOT_VOLT_TIME 50

/*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 start*/
/*A14 code for SR-AL6528V-01-40 by chenyulin at 20240919 start*/
#if IS_ENABLED(CONFIG_HQ_PROJECT_O8) || IS_ENABLED(CONFIG_HQ_PROJECT_O22)
/*A14 code for SR-AL6528V-01-40 by chenyulin at 20240919 end*/
	#define STORE_MODE_CAPACITY_MAX 70
	#define STORE_MODE_CAPACITY_MIN 60
	#define STORE_MODE_FCC_MAX 500000 /*uA*/
#endif 
/*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 end*/
/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
/* Discharge controller Info*/
#define GXY_NORMAL_CHARGING 0
#define GXY_DISCHARGE_INPUT_SUSPEND (1 << 0)
/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/
/*A06_V code for SR-AL7160V-01-175 by xiongxiaoliang at 20240920 start*/
enum batt_current_event_ss {
	SEC_BAT_CURRENT_EVENT_NONE = 0x00000,               /* Default value, nothing will be happen */
	SEC_BAT_CURRENT_EVENT_AFC = 0x00001,                /* Do not use this*/
	SEC_BAT_CURRENT_EVENT_CHARGE_DISABLE = 0x00002,     /* This event is for a head mount VR, It is not need */
	SEC_BAT_CURRENT_EVENT_LOW_TEMP_SWELLING = 0x00010,  /* battery temperature is lower than 18 celsius degree */
	SEC_BAT_CURRENT_EVENT_HIGH_TEMP_SWELLING = 0x00020, /* battery temperature is higher than 41 celsius degree */
	SEC_BAT_CURRENT_EVENT_USB_100MA = 0x00040,          /* USB 2.0 device under test is set in unconfigured state, not enumerate */
	SEC_BAT_CURRENT_EVENT_SLATE = 0x00800,
};
/*A06_V code for SR-AL7160V-01-175 by xiongxiaoliang at 20240920 end*/
/*A06_V code for  SR-AL7160V-01-171 by yexuedong at 20240911 start*/
enum batt_misc_event_ss {
	BATT_MISC_EVENT_NONE = 0x000000,			/* Default value, nothing will be happen */
	BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE = 0x00000001, 
	BATT_MISC_EVENT_TIMEOUT_OPEN_TYPE = 0x00000004,
	BATT_MISC_EVENT_HICCUP_TYPE = 0x00000020,
	BATT_MISC_EVENT_FULL_CAPACITY = 0x01000000,
};
/*A06_V code for  SR-AL7160V-01-171 by yexuedong at 20240911 end*/

enum bat_temp_state_enum {
	BAT_TEMP_LOW = 0,
	BAT_TEMP_NORMAL,
	BAT_TEMP_HIGH
};

enum DUAL_CHG_STAT {
	BOTH_EOC,
	STILL_CHG,
};

enum ADC_SOURCE {
	NULL_HANDLE,
	FROM_CHG_IC,
	FROM_CS_ADC,
};

enum TA_STATE {
	TA_INIT_FAIL,
	TA_CHECKING,
	TA_NOT_SUPPORT,
	TA_NOT_READY,
	TA_READY,
	TA_PD_PPS_READY,
};

enum adapter_protocol_state {
	FIRST_HANDSHAKE,
	RUN_ON_PD,
	RUN_ON_UFCS,
};

enum TA_CAP_STATE {
	APDO_TA,
	WO_APDO_TA,
	STD_TA,
	ONLY_APDO_TA,
};

enum chg_dev_notifier_events {
	EVENT_FULL,
	EVENT_RECHARGE,
	EVENT_DISCHARGE,
};

struct battery_thermal_protection_data {
	int sm;
	bool enable_min_charge_temp;
	int min_charge_temp;
	int min_charge_temp_plus_x_degree;
	int max_charge_temp;
	int max_charge_temp_minus_x_degree;
};

/* sw jeita */
/*A14 code for SR-AL6528V-01-125 by shanxinkai at 20240923 start*/
#define JEITA_TEMP_ABOVE_T4_CV	4240000
#define JEITA_TEMP_T3_TO_T4_CV	4240000
#define JEITA_TEMP_T2_TO_T3_CV	4400000
#define JEITA_TEMP_T1_TO_T2_CV	4400000
#define JEITA_TEMP_T0_TO_T1_CV	4400000
#define JEITA_TEMP_BELOW_T0_CV	4400000
/*A06_V code for SR-AL7160V-01-143 by xiongxiaoliang at 20240903 start*/
#define JEITA_TEMP_ABOVE_T4_CUR  0
#define JEITA_TEMP_T3_TO_T4_CUR  1750000
#define JEITA_TEMP_T2_TO_T3_CUR  2700000
#define JEITA_TEMP_T1_TO_T2_CUR  1500000
#define JEITA_TEMP_T0_TO_T1_CUR  500000
#define JEITA_TEMP_BELOW_T0_CUR  0

#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
/*A14 code for SR-AL6528V-01-125 by shanxinkai at 20240923 end*/

// cp_jeita_cc
#define JEITA_TEMP_ABOV_T4_CP_CC  1000000
#define JEITA_TEMP_T3_TO_T4_CP_CC 3800000
#define JEITA_TEMP_T2_TO_T3_CP_CC 6000000
#define JEITA_TEMP_T1_TO_T2_CP_CC 1300000
#define JEITA_TEMP_T0_TO_T1_CP_CC 1000000
#define JEITA_TEMP_BELOW_T0_CP_CC 1000000
#endif
/*A06_V code for SR-AL7160V-01-143 by xiongxiaoliang at 20240903 end*/
#define TEMP_T4_THRES  50
#define TEMP_T4_THRES_MINUS_X_DEGREE 47
#define TEMP_T3_THRES  45
#define TEMP_T3_THRES_MINUS_X_DEGREE 39
#define TEMP_T2_THRES  10
#define TEMP_T2_THRES_PLUS_X_DEGREE 16
#define TEMP_T1_THRES  0
#define TEMP_T1_THRES_PLUS_X_DEGREE 6
#define TEMP_T0_THRES  0
#define TEMP_T0_THRES_PLUS_X_DEGREE  0
#define TEMP_NEG_10_THRES 0

/*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 start*/
#define GXY_DISCHARGE_STORE_MODE (1 << 3)
/*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 end*/
/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
#define GXY_DISCHARGE_SOC_RECHG (1 << 4)
#define GXY_DISCHARGE_BATT_FULL_CAPACITY (1 << 2)
/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
#define GXY_SHIP_CYCLE 3
/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/

/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 start*/
#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
// cp thermal
// lcm off param
#define AP_TEMP_ABOVE_T4_CP_CC    2500000
#define AP_TEMP_T3_TO_T4_CP_CC    3400000
#define AP_TEMP_T2_TO_T3_CP_CC    3500000
#define AP_TEMP_T1_TO_T2_CP_CC    3900000
#define AP_TEMP_BELOW_T1_CP_CC    6000000
// lcm on param
#define AP_TEMP_T1_LCMON_CP_CC  700000
#define AP_TEMP_T2_LCMON_CP_CC  600000
#define AP_TEMP_T3_LCMON_CP_CC  500000
#define AP_TEMP_NORMAL_LCMON_CP_CC 6000000

// lcm off temp thresh
#define AP_TEMP_T4_CP_THRES  44
#define AP_TEMP_T3_CP_THRES  42
#define AP_TEMP_T2_CP_THRES  40
#define AP_TEMP_T1_CP_THRES  39
// lcm on temp thresh
#define AP_TEMP_T1_CP_THRES_LCMON 37
#define AP_TEMP_T2_CP_THRES_LCMON 43
#define AP_TEMP_T3_CP_THRES_LCMON 44
#endif
/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 end*/
/*
 * Software JEITA
 * T0: -10 degree Celsius
 * T1: 0 degree Celsius
 * T2: 10 degree Celsius
 * T3: 45 degree Celsius
 * T4: 50 degree Celsius
 */
enum sw_jeita_state_enum {
	TEMP_BELOW_T0 = 0,
	TEMP_T0_TO_T1,
	TEMP_T1_TO_T2,
	TEMP_T2_TO_T3,
	TEMP_T3_TO_T4,
	TEMP_ABOVE_T4
};

struct info_notifier_block {
	struct notifier_block nb;
	struct mtk_charger *info;
};

struct sw_jeita_data {
	int sm;
	int pre_sm;
	int cv;
	/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
	int cc;
	#endif
	/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 end*/
	bool charging;
	bool error_recovery_flag;
};

struct mtk_charger_algorithm {

	int (*do_algorithm)(struct mtk_charger *info);
	int (*enable_charging)(struct mtk_charger *info, bool en);
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
	int (*enable_port_charging)(struct mtk_charger *info, bool en);
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/
	int (*do_event)(struct notifier_block *nb, unsigned long ev, void *v);
	int (*do_dvchg1_event)(struct notifier_block *nb, unsigned long ev,
			       void *v);
	int (*do_dvchg2_event)(struct notifier_block *nb, unsigned long ev,
			       void *v);
	int (*do_hvdvchg1_event)(struct notifier_block *nb, unsigned long ev,
			       void *v);
	int (*do_hvdvchg2_event)(struct notifier_block *nb, unsigned long ev,
			       void *v);
	int (*change_current_setting)(struct mtk_charger *info);
	void *algo_data;
};

struct charger_custom_data {
	int battery_cv;	/* uv */
	int max_charger_voltage;
	int max_charger_voltage_setting;
	int min_charger_voltage;
	int vbus_sw_ovp_voltage;

	int usb_charger_current;
	int ac_charger_current;
	int ac_charger_input_current;
	int charging_host_charger_current;

	/* sw jeita */
	int jeita_temp_above_t4_cv;
	int jeita_temp_t3_to_t4_cv;
	int jeita_temp_t2_to_t3_cv;
	int jeita_temp_t1_to_t2_cv;
	int jeita_temp_t0_to_t1_cv;
	int jeita_temp_below_t0_cv;
	/*A06_V code for SR-AL7160V-01-143 by xiongxiaoliang at 20240903 start*/
	/*A14 code for SR-AL6528V-01-125 by shanxinkai at 20240923 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8) || IS_ENABLED(CONFIG_HQ_PROJECT_O22)
	/*A14 code for SR-AL6528V-01-125 by shanxinkai at 20240923 end*/
	int jeita_temp_above_t4_cur;
	int jeita_temp_t3_to_t4_cur;
	int jeita_temp_t2_to_t3_cur;
	int jeita_temp_t1_to_t2_cur;
	int jeita_temp_t0_to_t1_cur;
	int jeita_temp_below_t0_cur;
	#endif
	/*A06_V code for SR-AL7160V-01-143 by xiongxiaoliang at 20240903 end*/
	int temp_t4_thres;
	int temp_t4_thres_minus_x_degree;
	int temp_t3_thres;
	int temp_t3_thres_minus_x_degree;
	int temp_t2_thres;
	int temp_t2_thres_plus_x_degree;
	int temp_t1_thres;
	int temp_t1_thres_plus_x_degree;
	int temp_t0_thres;
	int temp_t0_thres_plus_x_degree;
	int temp_neg_10_thres;
	/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
	int ap_temp_above_t4_cp_cc;
	int ap_temp_t3_to_t4_cp_cc;
	int ap_temp_t2_to_t3_cp_cc;
	int ap_temp_t1_to_t2_cp_cc;
	int ap_temp_below_t1_cp_cc;
	int ap_temp_t1_lcmon_cp_cc;
	int ap_temp_t2_lcmon_cp_cc;
	int ap_temp_t3_lcmon_cp_cc;
	int ap_temp_normal_lcmon_cp_cc;
	int ap_temp_t4_cp_thres;
	int ap_temp_t3_cp_thres;
	int ap_temp_t2_cp_thres;
	int ap_temp_t1_cp_thres;
	int ap_temp_t1_cp_thres_lcmon;
	int ap_temp_t2_cp_thres_lcmon;
	int ap_temp_t3_cp_thres_lcmon;
	#endif
	/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 end*/
	/* battery temperature protection */
	int mtk_temperature_recharge_support;
	int max_charge_temp;
	int max_charge_temp_minus_x_degree;
	int min_charge_temp;
	int min_charge_temp_plus_x_degree;

	/* dynamic mivr */
	int min_charger_voltage_1;
	int min_charger_voltage_2;
	int max_dmivr_charger_current;
	/*A06_V code for SR-AL7160V-01-163 by xiongxiaoliang at 20240912 start*/
	/*A14 code for SR-AL6528V-01-26 by zhangziyi at 20240929 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8) || IS_ENABLED(CONFIG_HQ_PROJECT_O22)
	bool gxy_batt_aging_enable;
	struct range_data batt_cv_data[MAX_CV_ENTRIES];
	#endif
	/*A14 code for SR-AL6528V-01-26 by zhangziyi at 20240929 end*/
	/*A06_V code for SR-AL7160V-01-163 by xiongxiaoliang at 20240912 end*/
};

struct charger_data {
	int input_current_limit;
	int charging_current_limit;

	int force_charging_current;
	int thermal_input_current_limit;
	int thermal_charging_current_limit;
	bool thermal_throttle_record;
	int disable_charging_count;
	int input_current_limit_by_aicl;
	int junction_temp_min;
	int junction_temp_max;
};

enum chg_data_idx_enum {
	CHG1_SETTING,
	CHG2_SETTING,
	DVCHG1_SETTING,
	DVCHG2_SETTING,
	HVDVCHG1_SETTING,
	HVDVCHG2_SETTING,
	CHGS_SETTING_MAX,
};

struct gxy_charger_interface {
	/*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 start*/
	int (*dump_charger_ic)(struct mtk_charger *info);
	/*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 end*/
	/*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 start*/
	void (*wake_up_charger)(struct mtk_charger *info);
	/*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 end*/
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
	int (*set_ship_mode)(struct mtk_charger *info, bool en);
	int (*get_ship_mode)(struct mtk_charger *info);
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/
	/*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
	int (*batt_timetofull)(void);
	/*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
	/*A06_V code for SR-AL7160V-01-175 by xiongxiaoliang at 20240920 start*/
	int (*batt_current_event)(struct mtk_charger *info);
	/*A06_V code for SR-AL7160V-01-175 by xiongxiaoliang at 20240920 end*/
	/*A06_V code for  SR-AL7160V-01-171 by yexuedong at 20240911 start*/
	int (*batt_misc_event)(struct mtk_charger *info);
	/*A06_V code for  SR-AL7160V-01-171 by yexuedong at 20240911 end*/
	/*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 start*/
	void (*set_slate_mode)(struct mtk_charger *info, int slate_mode);
	/*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 end*/
};

struct mtk_charger {
	struct platform_device *pdev;
	struct charger_device *chg1_dev;
	struct notifier_block chg1_nb;
	struct charger_device *chg2_dev;
	struct charger_device *dvchg1_dev;
	struct notifier_block dvchg1_nb;
	struct charger_device *dvchg2_dev;
	struct notifier_block dvchg2_nb;
	struct charger_device *hvdvchg1_dev;
	struct notifier_block hvdvchg1_nb;
	struct charger_device *hvdvchg2_dev;
	struct notifier_block hvdvchg2_nb;
	struct charger_device *bkbstchg_dev;
	struct notifier_block bkbstchg_nb;
	struct charger_device *cschg1_dev;
	struct notifier_block cschg1_nb;
	struct charger_device *cschg2_dev;
	struct notifier_block cschg2_nb;


	struct charger_data chg_data[CHGS_SETTING_MAX];
	struct chg_limit_setting setting;
	enum charger_configuration config;

	struct power_supply_desc psy_desc1;
	struct power_supply_config psy_cfg1;
	struct power_supply *psy1;

	struct power_supply_desc psy_desc2;
	struct power_supply_config psy_cfg2;
	struct power_supply *psy2;

	struct power_supply_desc psy_dvchg_desc1;
	struct power_supply_config psy_dvchg_cfg1;
	struct power_supply *psy_dvchg1;

	struct power_supply_desc psy_dvchg_desc2;
	struct power_supply_config psy_dvchg_cfg2;
	struct power_supply *psy_dvchg2;

	struct power_supply_desc psy_hvdvchg_desc1;
	struct power_supply_config psy_hvdvchg_cfg1;
	struct power_supply *psy_hvdvchg1;

	struct power_supply_desc psy_hvdvchg_desc2;
	struct power_supply_config psy_hvdvchg_cfg2;
	struct power_supply *psy_hvdvchg2;
	/*A06_V code for SR-AL7160V-01-132 by xiongxiaoliang at 20240820 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
	struct power_supply_desc psy_pmic_desc;
	struct power_supply_config psy_pmic_cfg;
	struct power_supply *psy_pmic;
	struct iio_channel *chan_vbus;

	struct power_supply_desc psy_main1_desc;
	struct power_supply_config psy_main1_cfg;
	struct power_supply *psy_main1;

	struct power_supply_desc psy_main2_desc;
	struct power_supply_config psy_main2_cfg;
	struct power_supply *psy_main2;

	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 start*/
	struct power_supply_desc psy_main3_desc;
	struct power_supply_config psy_main3_cfg;
	struct power_supply *psy_main3;
	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 end*/
	#endif
	/*A06_V code for SR-AL7160V-01-132 by xiongxiaoliang at 20240820 end*/

	/*A14_V code for SR-AL6528V-01-117 by gaoxugang at 20240827 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O22)
	struct power_supply_desc psy_pmic_desc;
	struct power_supply_config psy_pmic_cfg;
	struct power_supply *psy_pmic;
	struct iio_channel *chan_vbus;

	struct power_supply_desc psy_main1_desc;
	struct power_supply_config psy_main1_cfg;
	struct power_supply *psy_main1;

	struct power_supply_desc psy_main2_desc;
	struct power_supply_config psy_main2_cfg;
	struct power_supply *psy_main2;

	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 start*/
	struct power_supply_desc psy_main3_desc;
	struct power_supply_config psy_main3_cfg;
	struct power_supply *psy_main3;
	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 end*/
	#endif
	/*A14_V code for SR-AL6528V-01-117 by gaoxugang at 20240827 end*/

	struct power_supply  *chg_psy;
	struct power_supply  *bc12_psy;
	struct power_supply  *bat_psy;
	struct power_supply  *bat2_psy;
	struct power_supply  *bat_manager_psy;
	struct adapter_device *select_adapter;
	struct adapter_device *pd_adapter;
	struct adapter_device *adapter_dev[MAX_TA_IDX];
	struct notifier_block *nb_addr;
	struct info_notifier_block ta_nb[MAX_TA_IDX];
	struct adapter_device *ufcs_adapter;
	struct mutex pd_lock;
	struct mutex ufcs_lock;
	struct mutex ta_lock;

	u32 bootmode;
	u32 boottype;

	int ta_status[MAX_TA_IDX];
	int select_adapter_idx;
	int ta_hardreset;
	int chr_type;
	int usb_type;
	int usb_state;
	int adapter_priority;
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
	int gxy_shipmode_flag;
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/
	/*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 start*/
	int gxy_tcpc_info;
	/*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 end*/
	/*A06_V code for SR-AL7160V-01-132 by xiongxiaoliang at 20240820 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
	int ac_online;
	int usb_online;
	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 start*/
	int otg_online;
	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 end*/
	#endif
	/*A06_V code for SR-AL7160V-01-132 by xiongxiaoliang at 20240820 end*/
	/*A14_V code for SR-AL6528V-01-117 by gaoxugang at 20240827 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O22)
	int ac_online;
	int usb_online;
	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 start*/
	int otg_online;
	/*A06_V code for SR-AL7160V-01-101 by yexuedong at 20240909 end*/
	#endif
	/*A14_V code for SR-AL6528V-01-117 by gaoxugang at 20240827 end*/

	struct mutex cable_out_lock;
	int cable_out_cnt;

	/* system lock */
	spinlock_t slock;
	struct wakeup_source *charger_wakelock;
	struct mutex charger_lock;

	/* thread related */
	wait_queue_head_t  wait_que;
	bool charger_thread_timeout;
	unsigned int polling_interval;
	bool charger_thread_polling;

	/* alarm timer */
	struct alarm charger_timer;
	struct timespec64 endtime;
	bool is_suspend;
	struct notifier_block pm_notifier;

	/* notify charger user */
	struct srcu_notifier_head evt_nh;

	/* common info */
	int log_level;
	bool usb_unlimited;
	bool charger_unlimited;
	bool disable_charger;
	bool disable_aicl;
	int battery_temp;
	bool can_charging;
	bool cmd_discharging;
	bool safety_timeout;
	int safety_timer_cmd;
	bool vbusov_stat;
	bool dpdmov_stat;
	bool lst_dpdmov_stat;
	bool is_chg_done;
	/*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 start*/
	bool gxy_discharge_store_mode;
	bool gxy_discharge_store_mode_flag;
	int store_mode_charging_max;
	int store_mode_charging_min;
	/*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 end*/
	/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
	int gxy_discharge_batt_full_flag;
	/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/

	/* ATM */
	bool atm_enabled;

	const char *algorithm_name;
	const char *curr_select_name;
	struct mtk_charger_algorithm algo;
	/*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 start*/
	struct gxy_charger_interface gxy_cint;
	/*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 end*/
	/* dtsi custom data */
	struct charger_custom_data data;

	/* battery warning */
	unsigned int notify_code;
	unsigned int notify_test_mode;

	/* sw safety timer */
	bool enable_sw_safety_timer;
	bool sw_safety_timer_setting;
	struct timespec64 charging_begin_time;

	/* vbat monitor, 6pin bat */
	bool batpro_done;
	bool enable_vbat_mon;
	bool enable_vbat_mon_bak;
	int old_cv;
	bool stop_6pin_re_en;
	int vbat0_flag;

	/* sw jeita */
	bool enable_sw_jeita;
	struct sw_jeita_data sw_jeita;

	/* battery thermal protection */
	struct battery_thermal_protection_data thermal;

	struct chg_alg_device *alg[MAX_ALG_NO];
	int lst_rnd_alg_idx;
	bool alg_new_arbitration;
	bool alg_unchangeable;
	struct notifier_block chg_alg_nb;
	bool enable_hv_charging;

	/* water detection */
	bool water_detected;
	bool record_water_detected;

	bool enable_dynamic_mivr;

	/* fast charging algo support indicator */
	bool enable_fast_charging_indicator;
	unsigned int fast_charging_indicator;

	/* diasable meta current limit for testing */
	unsigned int enable_meta_current_limit;

	/* set current selector parallel mode */
	int cs_heatlim;
	unsigned int cs_para_mode;
	int cs_gpio_index;
	bool cs_hw_disable;
	int dual_chg_stat;
	int cs_cc_now;
	int comp_resist;
	struct smartcharging sc;
	bool cs_with_gauge;

	/*daemon related*/
	struct sock *daemo_nl_sk;
	u_int g_scd_pid;
	struct scd_cmd_param_t_1 sc_data;

	/*charger IC charging status*/
	bool is_charging;
	bool is_cs_chg_done;
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
	bool port_can_charging;
	int gxy_discharge_info;
	int gxy_discharge_input_suspend;
	/*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/

	ktime_t uevent_time_check;

	bool force_disable_pp[CHG2_SETTING + 1];
	bool enable_pp[CHG2_SETTING + 1];
	struct mutex pp_lock[CHG2_SETTING + 1];
	int cmd_pp;

	/* enable boot volt*/
	bool enable_boot_volt;
	bool reset_boot_volt_times;

	/* adapter switch control */
	int protocol_state;
	int ta_capability;
	int wait_times;
	/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
	bool lcm_sts;
	int ap_temp;
	struct sw_jeita_data ap_thermal_lcmoff;
	struct sw_jeita_data ap_thermal_lcmon;
	#endif
	/*A06_V code for SR-AL7160V-01-201 by xiongxiaoliang at 20240911 end*/
	/*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
	int afc_result;
	int gxy_discharge_batt_full_capacity;
	/*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
	/*A14_V code for P241227-03268 by chenyulin at 20250110 start*/
	int gxy_batt_max_protect_soc;
	/*A14_V code for P241227-03268 by chenyulin at 20250110 end*/
	/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
	bool gxy_batt_soc_rechg;
	bool gxy_batt_soc_rechg_flag;
	/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
	/*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 start*/
	int chg_pd_type;
	/*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 end*/
	/*A06_V code for AL7160AV-48 by xiongxiaoliang at 20241113 start*/
        /*A14 code for AL6528AV-20 by chenyulin at 20241114 start*/
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8) || IS_ENABLED(CONFIG_HQ_PROJECT_O22)
        /*A14 code for AL6528AV-20 by chenyulin at 20241114 end*/
	bool hv_voltage_switch1;
	#endif
	/*A06_V code for AL7160AV-48 by xiongxiaoliang at 20241113 end*/
	/*A14 code add for P241227-01958 by chenyulin at 20250106 start*/
        bool enable_afc_charging;
	/*A14 code add for P241227-01958 by chenyulin at 20250106 end*/
};

static inline int mtk_chg_alg_notify_call(struct mtk_charger *info,
					  enum chg_alg_notifier_events evt,
					  int value)
{
	int i;
	struct chg_alg_notify notify = {
		.evt = evt,
		.value = value,
	};

	for (i = 0; i < MAX_ALG_NO; i++) {
		if (info->alg[i])
			chg_alg_notifier_call(info->alg[i], &notify);
	}
	return 0;
}

/* functions which framework needs*/
extern int mtk_basic_charger_init(struct mtk_charger *info);
extern int mtk_pulse_charger_init(struct mtk_charger *info);
extern int get_uisoc(struct mtk_charger *info);
extern int get_battery_voltage(struct mtk_charger *info);
extern int get_battery_temperature(struct mtk_charger *info);
extern int get_battery_current(struct mtk_charger *info);
extern int get_cs_side_battery_current(struct mtk_charger *info, int *ibat);
extern int get_cs_side_battery_voltage(struct mtk_charger *info, int *vbat);
extern int get_chg_output_vbat(struct mtk_charger *info, int *vbat);
extern int get_vbus(struct mtk_charger *info);
extern int get_ibat(struct mtk_charger *info);
extern int get_ibus(struct mtk_charger *info);
extern bool is_battery_exist(struct mtk_charger *info);
extern int get_charger_type(struct mtk_charger *info);
extern int get_usb_type(struct mtk_charger *info);
extern int disable_hw_ovp(struct mtk_charger *info, int en);
extern bool is_charger_exist(struct mtk_charger *info);
extern int get_charger_temperature(struct mtk_charger *info,
	struct charger_device *chg);
extern int get_charger_charging_current(struct mtk_charger *info,
	struct charger_device *chg);
extern int get_charger_input_current(struct mtk_charger *info,
	struct charger_device *chg);
extern int get_charger_zcv(struct mtk_charger *info,
	struct charger_device *chg);
extern void _wake_up_charger(struct mtk_charger *info);
extern int mtk_adapter_switch_control(struct mtk_charger *info);
extern int mtk_selected_adapter_ready(struct mtk_charger *info);
extern int mtk_adapter_protocol_init(struct mtk_charger *info);
extern void mtk_check_ta_status(struct mtk_charger *info);
/* functions for other */
extern int mtk_chg_enable_vbus_ovp(bool enable);

#define ONLINE(idx, attach)		((idx & 0xf) << 4 | (attach & 0xf))
#define ONLINE_GET_IDX(online)		((online >> 4) & 0xf)
#define ONLINE_GET_ATTACH(online)	(online & 0xf)

#endif /* __MTK_CHARGER_H */
