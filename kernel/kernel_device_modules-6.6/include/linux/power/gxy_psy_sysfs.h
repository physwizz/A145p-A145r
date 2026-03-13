/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __GXY_PSY_SYSFS_H__
#define __GXY_PSY_SYSFS_H__
#include <linux/power_supply.h>
#include <linux/sysfs.h>
/*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 start*/
#define GXY_PSY_TAG    "[GXY_PSY]"
#define GXY_PSY_DEBUG 1
#ifdef GXY_PSY_DEBUG
    #define GXY_PSY_INFO(fmt, args...)    pr_info(GXY_PSY_TAG fmt, ##args)
#else
    #define GXY_PSY_INFO(fmt, args...)
#endif
/*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 end*/
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
#define GXY_PSY_ERR(fmt, args...)    pr_err(GXY_PSY_TAG fmt, ##args)
#define DATA_MAXSIZE 32
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
/* log debug */
/* battery psy attrs */
ssize_t gxy_bat_show_attrs(struct device *dev,
                            struct device_attribute *attr, char *buf);
ssize_t gxy_bat_store_attrs(struct device *dev,
                struct device_attribute *attr,
                const char *buf, size_t count);

ssize_t gxy_usb_show_attrs(struct device *dev,
                            struct device_attribute *attr, char *buf);
ssize_t gxy_usb_store_attrs(struct device *dev,
                struct device_attribute *attr,
                const char *buf, size_t count);

#define GXY_BATTERY_ATTR(_name)                              \
{                                                      \
    .attr = {.name = #_name, .mode = 0664},        \
    .show = gxy_bat_show_attrs,                      \
    .store = gxy_bat_store_attrs,                      \
}

#define GXY_USB_ATTR(_name)                              \
{                                                      \
    .attr = {.name = #_name, .mode =  0664},        \
    .show = gxy_usb_show_attrs,                      \
    .store = gxy_usb_store_attrs,                      \
}
/* battery psy property */
enum {
    BATTERY_TYPE = 0,
    CHG_INFO,
    TCPC_INFO,
    BATT_CAP_CONTROL,
    BATT_FULL_CAPACITY,
    STORE_MODE,
    BATT_SLATE_MODE,
    INPUT_SUSPEND,
    AFC_RESULT,
    HV_DISABLE,
    HV_CHARGER_STATUS,
    SHIPMODE,
    SHIPMODE_REG,
    BATT_MISC_EVENT,
    BATT_CURRENT_EVENT,
    BATT_TEMP,
    BATT_DISCHARGE_LEVEL,
    BATT_TYPE,
    BATT_CURRENT_UA_NOW,
    BATTERY_CYCLE,
    RESET_BATTERY_CYCLE,
    /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
    BATTERY_CYCLE_DEBUG,
    /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
    CHARGE_TYPE,
    DUMP_CHARGER_IC,
    /*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 start*/
    ONLINE,
    /*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 end*/
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
    TIME_TO_FULL_NOW,
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
    /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
    BATT_SOC_RECHG,
    /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
    /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 start*/
    #if IS_ENABLED(CONFIG_CHARGER_CP_PPS)
    DIRECT_CHARGING_STATUS,
    CHARGING_TYPE,
    #endif
    /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 end*/
    /*A06 code add for AL7160AV-91 by chenyulin at 20250101 start*/
    CP_INFO,
    /*A06 code add for AL7160AV-91 by chenyulin at 20250101 end*/
};

/*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 start*/
/* AFC detected */
enum {
    AFC_INIT,
    NOT_AFC,
    AFC_FAIL,
    AFC_DISABLE,
    NON_AFC_MAX,
    AFC_5V = NON_AFC_MAX,
    AFC_9V,
    AFC_12V,
};
/*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 end*/

enum {
    TYPEC_CC_ORIENT = 0,
};

/* battey psy property - bat_info data */
enum gxy_battery_type {
    GXY_BATTERY_TYPE_SDI = 0,
    GXY_BATTERY_TYPE_BYD,
    GXY_BATTERY_TYPE_ATL,
    GXY_BATTERY_TYPE_GF,
    GXY_BATTERY_TYPE_UNKNOWN,
};
/*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 start*/
enum gxy_bat_chg_info {
    GXY_BAT_CHG_INFO_UNKNOWN = 0,
    GXY_BAT_CHG_INFO_SGM41513DYTQF24G,
    GXY_BAT_CHG_INFO_UPM6922,
    GXY_BAT_CHG_INFO_CX25601D,
    GXY_BAT_CHG_INFO_UPM6922P,
    /*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 start*/
    GXY_BAT_CHG_INFO_SC8960X,
    /*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 end*/
    /*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 start*/
    GXY_BAT_CHG_INFO_UPM6910X,
    /*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 end*/
};
/*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 end*/
/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 start*/
enum gxy_bat_tcpc_info {
    GXY_BAT_TCPC_INFO_UNKNOWN = 0,
    GXY_BAT_TCPC_INFO_CPS8851MRE,
    GXY_BAT_TCPC_INFO_UPM7610,
    GXY_BAT_TCPC_INFO_WUSB3812C,
    GXY_BAT_TCPC_INFO_UPM6922P,
    /*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 start*/
    GXY_BAT_TCPC_INFO_HUSB311,
    GXY_BAT_TCPC_INFO_ET7304,
    /*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 end*/
    /*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 start*/
    GXY_BAT_TCPC_INFO_FUSB302B,
    /*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 end*/
};
/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 end*/
/*A06 code add for AL7160AV-91 by chenyulin at 20250101 start*/
enum gxy_bat_cp_info {
    GXY_BAT_CP_INFO_UNKNOWN = 0,
    GXY_BAT_CP_INFO_NX8530,
    GXY_BAT_CP_INFO_NU2115AWCNB,
    GXY_BAT_CP_INFO_UPM6720,
    GXY_BAT_CP_INFO_SGM41606S,
};
/*A06 code add for AL7160AV-91 by chenyulin at 20250101 end*/
/* data struct type */
struct gxy_bat_hwinfo {
    enum gxy_battery_type binfo;
    /*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 start*/
    enum gxy_bat_chg_info cinfo;
    /*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 end*/
    /*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 start*/
    enum gxy_bat_tcpc_info tinfo;
    /*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 end*/
    /*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 start*/
    int afc_result;
    /*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 end*/
    /*A06 code for AL7160AV-91 by chenyulin at 20250101 start*/
    enum gxy_bat_cp_info pinfo;
    /*A06 code for AL7160AV-91 by chenyulin at 20250101 end*/
};
struct gxy_battery {
    struct power_supply *psy;
    struct power_supply *usb_psy;
};
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
struct gxy_battery_data {
    char cust_batt_type[DATA_MAXSIZE];
};
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
/*definition for batt_full_capacity*/
#define GXY_FULL_CAPACITY_RECHG_VALUE 2
#define GXY_FULL_CAPACITY_LEVEL                   100
#define GXY_PROTECT_CAPACITY_LEVEL          80
#define GXY_MAX_PROTECTION_FLAG               1   // port_charging on, charging off
#define GXY_SLEEP_PROTECTION_FLAG           2   // port_charging on, charging off
#define GXY_HIGHSOC_PROTECTION_FLAG    3   // port_charging off, charging off
static const char *gs_basic_protection  = "100";
/*A14_V code for P241227-03268 by chenyulin at 20250110 start*/
static const char *gs_maximum_protection  = "OPTION";
/*A14_V code for P241227-03268 by chenyulin at 20250110 end*/
static const char *gs_sleep_protection  = "80 SLEEP";
static const char *gs_highsoc_protection  = "80 HIGHSOC";
#define GXY_BASIC_PROTECT_RECHG_SOC 95
/*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
#endif /* __GXY_PSY_SYSFS_H__ */
