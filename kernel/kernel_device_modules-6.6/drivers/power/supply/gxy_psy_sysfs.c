/*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 start*/
#define pr_fmt(fmt) "[gxy_psy]: " fmt
/*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 end*/
// SPDX-License-Identifier: GPL-2.0
#include <linux/err.h>    /* IS_ERR, PTR_ERR */
#include <linux/init.h>        /* For init/exit macros */
#include <linux/kernel.h>
#include <linux/module.h>    /* For MODULE_ marcros  */
#include <linux/of_fdt.h>    /*of_dt API*/
#include <linux/of.h>
#include <linux/platform_device.h>    /* platform device */
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/power/gxy_psy_sysfs.h>
#include "mtk_battery.h"
#include "mtk_charger.h"
/*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 start*/
#include "../../misc/mediatek/typec/tcpc/inc/tcpm.h"
/*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 end*/

static struct gxy_bat_hwinfo s_hwinfo_data;
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
static struct gxy_battery_data data_t;
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
/* For Bat_info requirement*/
static const char * const GXY_BATTERY_TYPE_TEXT[] = {
    [GXY_BATTERY_TYPE_SDI] = "1:battery-SDI",
    [GXY_BATTERY_TYPE_BYD] = "2:battery-BYD",
    [GXY_BATTERY_TYPE_ATL] = "3:battery-ATL",
    [GXY_BATTERY_TYPE_GF] = "4:battery-GF",
    [GXY_BATTERY_TYPE_UNKNOWN] = "UNKNOWN",
};
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
static const char * const GXY_CHARGE_TYPE_TEXT[] = {
    [POWER_SUPPLY_CHARGE_TYPE_UNKNOWN]     = "Unknown",
    [POWER_SUPPLY_CHARGE_TYPE_NONE]        = "N/A",
    [POWER_SUPPLY_CHARGE_TYPE_TRICKLE]     = "Trickle",
    [POWER_SUPPLY_CHARGE_TYPE_FAST]        = "Fast",
    //[POWER_SUPPLY_CHARGE_TYPE_TAPER]       = "Taper",
};
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
/*A14 code for AL6528AV-26 by chenyulin at 20241210 start*/
enum {
	SLATE_DISABLE = 0,
	SLATE_ENABLE,
	SMART_SWITCH_SLATE,
	SMART_SRC,
};
/*A14 code for AL6528AV-26 by chenyulin at 20241210 end*/
/*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 start*/
#if IS_ENABLED(CONFIG_CHARGER_CP_PPS)
enum gxy_charging_type {
    GXY_CHARGING_TYPE_NONE = 0,
    GXY_CHARGING_TYPE_UNDEFINED,
    GXY_CHARGING_TYPE_USB,
    GXY_CHARGING_TYPE_USB_CDP,
    GXY_CHARGING_TYPE_TA,
    GXY_CHARGING_TYPE_9V_TA,
    GXY_CHARGING_TYPE_PDIC_APDO,
};

static const char * const GXY_CHARGING_TYPE_TEXT[] = {
    [GXY_CHARGING_TYPE_NONE]       = "NONE",
    [GXY_CHARGING_TYPE_UNDEFINED]  = "UNDEFINED",
    [GXY_CHARGING_TYPE_USB]        = "USB",
    [GXY_CHARGING_TYPE_USB_CDP]    = "USB_CDP",
    [GXY_CHARGING_TYPE_TA]         = "TA",
    [GXY_CHARGING_TYPE_9V_TA]      = "9V_TA",
    [GXY_CHARGING_TYPE_PDIC_APDO]  = "PDIC_APDO",
};
#endif
/*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 end*/
void gxy_bat_set_batterytype(enum gxy_battery_type binfo_data)
{
    s_hwinfo_data.binfo = binfo_data;
}
EXPORT_SYMBOL(gxy_bat_set_batterytype);
/*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 start*/
static const char * const GXY_BAT_CHG_INFO_TEXT[] = {
    [GXY_BAT_CHG_INFO_UNKNOWN]  =  "charger-Unknown",
    [GXY_BAT_CHG_INFO_SGM41513DYTQF24G]  =  "1:charger-sgm41513d",
    [GXY_BAT_CHG_INFO_UPM6922]  =  "2:charger-upm6922",
    [GXY_BAT_CHG_INFO_CX25601D]  = "3:charger-cx25601d",
    [GXY_BAT_CHG_INFO_UPM6922P]  = "4:charger-upm6922p",
    /*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 start*/
    [GXY_BAT_CHG_INFO_SC8960X]  =  "5:charger-sc8960x",
    /*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 end*/
    /*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 start*/
    [GXY_BAT_CHG_INFO_UPM6910X] =  "6:charger-upm6910x",
    /*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 end*/
};

void gxy_bat_set_chginfo(enum gxy_bat_chg_info cinfo_data)
{
    s_hwinfo_data.cinfo = cinfo_data;
}
EXPORT_SYMBOL(gxy_bat_set_chginfo);
/*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 end*/
/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 start*/
/* For tcpc_info requirement*/
static const char * const GXY_BAT_TCPC_INFO_TEXT[] = {
    [GXY_BAT_TCPC_INFO_UNKNOWN]  =  "tcpc-Unknown",
    [GXY_BAT_TCPC_INFO_CPS8851MRE]  =  "1:tcpc-CPS8851MRE",
    [GXY_BAT_TCPC_INFO_UPM7610]  =  "2:tcpc-UPM7610",
    [GXY_BAT_TCPC_INFO_WUSB3812C]  = "3:tcpc-WUSB3812C",
    [GXY_BAT_TCPC_INFO_UPM6922P]="4:tcpc-UPM6922P",
/*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 start*/
    [GXY_BAT_TCPC_INFO_HUSB311]  =  "5:tcpc-husb311",
    [GXY_BAT_TCPC_INFO_ET7304]  =   "6:tcpc-et7304",
/*A14 code for SR-AL6528V-01-111 by chenyulin at 20240911 end*/
/*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 start*/
    [GXY_BAT_TCPC_INFO_FUSB302B] =  "7:tcpc-fusb302b",
/*A14 code for SR-AL6528V-01-112 by chenyulin at 20240913 end*/
};
void gxy_bat_set_tcpcinfo(enum gxy_bat_tcpc_info tinfo_data)
{
    s_hwinfo_data.tinfo = tinfo_data;
}
EXPORT_SYMBOL(gxy_bat_set_tcpcinfo);
/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 end*/
/*A06 code add for AL7160AV-91 by chenyulin at 20250101 start*/
static const char * const GXY_BAT_CP_INFO_TEXT[] = {
    [GXY_BAT_CP_INFO_UNKNOWN]  =  "cp-Unknown",
    [GXY_BAT_CP_INFO_NX8530]  =  "1:cp-nx8530",
    [GXY_BAT_CP_INFO_NU2115AWCNB]  =  "2:cp-nu2115awcnb",
    [GXY_BAT_CP_INFO_UPM6720]  = "3:cp-upm6720",
    [GXY_BAT_CP_INFO_SGM41606S] = "4:cp-sgm41606s",
};
void gxy_bat_set_cpinfo(enum gxy_bat_cp_info pinfo_data)
{
    s_hwinfo_data.pinfo = pinfo_data;
}
EXPORT_SYMBOL(gxy_bat_set_cpinfo);
/*A06 code add for AL7160AV-91 by chenyulin at 20250101 end*/
/*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 start*/
static void gxy_batt_set_slate_mode(int slate_mode)
{
    static struct mtk_charger *s_info = NULL;
    struct power_supply *psy = NULL;
    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get mtk-master-charger psy failed\n", __func__);
            return;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: get mtk_charger failed\n", __func__);
                return;
            }
        }
    }
    GXY_PSY_INFO("%s: set slate_mode = (%d)\n", __func__, slate_mode);
    /*A14 code for AL6528AV-26 by chenyulin at 20241210 start*/
    if(slate_mode == SLATE_ENABLE || slate_mode == SMART_SWITCH_SLATE) {
        s_info->gxy_discharge_input_suspend = true;
    }else if(slate_mode == SLATE_DISABLE) {
        s_info->gxy_discharge_input_suspend = false;
    }
    /*A14 code for AL6528AV-26 by chenyulin at 20241210 end*/
    s_info->gxy_tcpc_info = s_hwinfo_data.tinfo;
    s_info->gxy_cint.set_slate_mode(s_info, slate_mode);
}
/*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 end*/
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
static void gxy_set_bat_cycle_debug(int val)
{
    static struct mtk_battery_manager *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            return;
        } else {
            s_info = (struct mtk_battery_manager *)power_supply_get_drvdata(psy);
            power_supply_put(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: mtk_battery_manager is not rdy\n", __func__);
                return;
            }
        }
    }

    if (s_info->gm1 != NULL) {
        GXY_PSY_ERR("%s: set bat_cycle_debug = %d\n", __func__, s_info->gm1->bat_cycle_debug);
        s_info->gm1->bat_cycle_debug = val;
    } else {
        GXY_PSY_ERR("%s: s_info->gm1 NULL\n", __func__);
    }
}

static int gxy_get_bat_cycle_debug(void)
{
    static struct mtk_battery_manager *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            goto out;
        } else {
            s_info = (struct mtk_battery_manager *)power_supply_get_drvdata(psy);
            power_supply_put(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: mtk_battery_manager is not rdy\n", __func__);
                goto out;
            }
        }
    }

    if (s_info->gm1 != NULL) {
        GXY_PSY_ERR("%s: get bat_cycle_debug = %d\n", __func__, s_info->gm1->bat_cycle_debug);
        return s_info->gm1->bat_cycle_debug;
    }

out:
    return 0;
}

static int gxy_get_chr_type(void)
{
    static struct power_supply *psy = NULL;
    union power_supply_propval prop = {0};
    int ret = 0;
    int chr_type = POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;

    if (psy == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            return chr_type;
        }
    }
    ret = power_supply_get_property(psy, POWER_SUPPLY_PROP_STATUS, &prop);
    if (ret < 0) {
        GXY_PSY_ERR("%s:get battery status property fail\n", __func__);
        return chr_type;
    }
    switch (prop.intval) {
        case POWER_SUPPLY_STATUS_DISCHARGING:
            chr_type = POWER_SUPPLY_CHARGE_TYPE_NONE;
            break;
        case POWER_SUPPLY_STATUS_CHARGING:
        case POWER_SUPPLY_STATUS_FULL:
            chr_type = POWER_SUPPLY_CHARGE_TYPE_FAST;
            break;
        default:
            chr_type = POWER_SUPPLY_CHARGE_TYPE_NONE;
            break;

    }
    return chr_type;
}

static int gxy_get_bat_cycle(void)
{
    static struct mtk_battery_manager *s_info = NULL;
    struct power_supply *psy = NULL;
    int batt_cycle = 0;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            goto out;
        } else {
            s_info = (struct mtk_battery_manager *)power_supply_get_drvdata(psy);
            power_supply_put(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: mtk_battery_manager is not rdy\n", __func__);
                goto out;
            }
        }
    }

    if (s_info->gm1 != NULL) {
       batt_cycle = s_info->gm1->bat_cycle;
    } else {
        GXY_PSY_ERR("%s: s_info->gm1 NULL\n", __func__);
    }
    GXY_PSY_ERR("%s: get bat_cycle = %d\n", __func__, batt_cycle);

out:
    return batt_cycle;
}

static int gxy_bat_get_temp(void)
{
    static struct power_supply *s_batt_psy = NULL;
    int ret = 0;
    union power_supply_propval prop = {0};
    const int normal_temp = 250; // 25 degree

    if (s_batt_psy == NULL) {
        s_batt_psy = power_supply_get_by_name("battery");
        if (s_batt_psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            return normal_temp;
        }
    }
    ret = power_supply_get_property(s_batt_psy, POWER_SUPPLY_PROP_TEMP, &prop);
    if (ret < 0) {
        GXY_PSY_ERR("%s:get temp property fail\n", __func__);
        return normal_temp;
    }
    return prop.intval;
}

static int gxy_bat_get_current_now(void)
{
    static struct power_supply *s_batt_psy = NULL;
    int ret = 0;
    union power_supply_propval prop = {0};
    const int current_now = 50000; // 500ma

    if (s_batt_psy == NULL) {
        s_batt_psy = power_supply_get_by_name("battery");
        if (s_batt_psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            return current_now;
        }
    }
    ret = power_supply_get_property(s_batt_psy, POWER_SUPPLY_PROP_CURRENT_NOW, &prop);
    if (ret < 0) {
        GXY_PSY_ERR("%s:get temp property fail\n", __func__);
        return current_now;
    }
    return prop.intval;
}

static int gxy_get_bat_discharge_level(void)
{
    static struct mtk_battery_manager *s_info = NULL;
    struct power_supply *psy = NULL;
    int bat_discharge_level = 0;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            goto out;
        } else {
            s_info = (struct mtk_battery_manager *)power_supply_get_drvdata(psy);
            power_supply_put(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: mtk_battery_manager is not rdy\n", __func__);
                goto out;
            }
        }
    }

    if (s_info->gm1 != NULL && s_info->gm1->gauge != NULL) {
        bat_discharge_level = (s_info->gm1->bat_cycle * 100) +
                (abs(s_info->gm1->gauge->fg_hw_info.ncar) * 100 / s_info->gm1->bat_cycle_thr);

        pr_err("%s: get b_c = %d, get ncar = %d, get b_c_thr = %d, get b_d_l = %d\n",
                __func__, s_info->gm1->bat_cycle, s_info->gm1->gauge->fg_hw_info.ncar,
                s_info->gm1->bat_cycle_thr, bat_discharge_level);
    } else {
        GXY_PSY_ERR("%s: s_info->gm1 or s_info->gm1->gauge is NULL\n", __func__);
    }

out:
    return bat_discharge_level;
}

static void gxy_batt_set_reset_cycle(bool val)
{
    static struct mtk_battery_manager *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            return;
        } else {
            s_info = (struct mtk_battery_manager *)power_supply_get_drvdata(psy);
            power_supply_put(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: mtk_battery_manager is not rdy\n", __func__);
                return;
            }
        }
    }

    if (s_info->gm1 != NULL && s_info->gm1->reset_cycle != NULL) {
        GXY_PSY_ERR("%s: s_info->reset_cycle\n", __func__);
        s_info->gm1->reset_cycle(s_info->gm1, val);
    } else {
        GXY_PSY_ERR("%s: s_info->gm1 or s_info->gm1->reset_cycle null\n", __func__);
    }
}

static int gxy_batt_get_reset_cycle(void)
{
    static struct mtk_battery_manager *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("battery");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get battery psy failed\n", __func__);
            goto out;
        } else {
            s_info = (struct mtk_battery_manager *)power_supply_get_drvdata(psy);
            power_supply_put(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: mtk_battery_manager is not rdy\n", __func__);
                goto out;
            }
        }
    }

    if (s_info->gm1 != NULL) {
        pr_err("%s: get reset_cycle = %d\n", __func__, s_info->gm1->is_reset_battery_cycle);
        return s_info->gm1->is_reset_battery_cycle;
    } else {
        GXY_PSY_ERR("%s: s_info->gm1 NULL\n", __func__);
    }

out:
    return -EINVAL;
}
/*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
/*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 start*/
static int gxy_get_batt_online(void)
{
    struct power_supply *psy = NULL;
    union power_supply_propval prop_type = {0};
    union power_supply_propval val = {0};
    int ret = 0;

    psy = power_supply_get_by_name("charger");
    if (!psy) {
        GXY_PSY_ERR("%s charger psy is NULL\n", __func__);
        val.intval = POWER_SUPPLY_TYPE_BATTERY;
        return -EINVAL;
    }

    ret = power_supply_get_property(psy,
            POWER_SUPPLY_PROP_ONLINE, &prop_type);
    if (!prop_type.intval) {
        val.intval = POWER_SUPPLY_TYPE_BATTERY;
    } else {
        power_supply_get_property(psy,
            POWER_SUPPLY_PROP_USB_TYPE, &val);
        switch (val.intval) {
            case POWER_SUPPLY_USB_TYPE_SDP:
                val.intval = POWER_SUPPLY_TYPE_USB;
                break;
            case POWER_SUPPLY_USB_TYPE_DCP:
                val.intval = POWER_SUPPLY_TYPE_USB_DCP;
                break;
            case POWER_SUPPLY_USB_TYPE_CDP:
                val.intval = POWER_SUPPLY_TYPE_USB_CDP;
                break;
            case POWER_SUPPLY_USB_TYPE_UNKNOWN:
                val.intval = POWER_SUPPLY_TYPE_UNKNOWN;
                break;
        default:
            val.intval = POWER_SUPPLY_TYPE_UNKNOWN;
            break;
        }
    }

    ret = val.intval;
    GXY_PSY_INFO("%s: batt online: %d\n", __func__, val.intval);
    return ret;
}
/*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 end*/
/*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 start*/
#if IS_ENABLED(CONFIG_CHARGER_CP_PPS)
static int gxy_get_direct_charging_status(void)
{
    struct power_supply *psy = NULL;
    struct power_supply *ac_psy = NULL;
    union power_supply_propval val = {0};
    static struct mtk_charger *s_info = NULL;
    int ret = 0;
    int result = 0;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get mtk-master-charger psy failed\n", __func__);
            return -EINVAL;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: get mtk_charger failed\n", __func__);
                return -EINVAL;
            }
        }
    }

    ac_psy = power_supply_get_by_name("ac");
    if (!ac_psy) {
        GXY_PSY_ERR("%s ac psy is NULL\n", __func__);
        return -EINVAL;
    }

    ret = power_supply_get_property(ac_psy,
        POWER_SUPPLY_PROP_ONLINE, &val);
    if (ret < 0) {
        GXY_PSY_ERR("%s:get ac/online property fail\n", __func__);
        return ret;
    }

    if (!s_info->enable_hv_charging) {
        result = 0;
    } else {
        if ((val.intval == 1) &&
            (s_info->chg_pd_type == PD_CONNECT_PE_READY_SNK_APDO)) {
            result = 1;
        }
    }

    GXY_PSY_ERR("%s:result:%d\n", __func__, result);
    return result;
}

static int gxy_get_charging_type(void)
{
    struct power_supply *psy = NULL;
    struct power_supply *ac_psy = NULL;
    struct power_supply *chg_psy = NULL;
    union power_supply_propval ac_online = {0};
    union power_supply_propval chg_usb_type = {0};
    static struct mtk_charger *s_info = NULL;
    int ret = 0;
    int result = GXY_CHARGING_TYPE_NONE;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get mtk-master-charger psy failed\n", __func__);
            return -EINVAL;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: get mtk_charger failed\n", __func__);
                return -EINVAL;
            }
        }
    }

    ac_psy = power_supply_get_by_name("ac");
    if (!ac_psy) {
        GXY_PSY_ERR("%s ac psy is NULL\n", __func__);
        return -EINVAL;
    }

    ret = power_supply_get_property(ac_psy,
        POWER_SUPPLY_PROP_ONLINE, &ac_online);
    if (ret < 0) {
        GXY_PSY_ERR("%s:get ac/online property fail\n", __func__);
        return ret;
    }

    chg_psy = power_supply_get_by_name("charger");
    if (!chg_psy) {
        GXY_PSY_ERR("%s charger psy is NULL\n", __func__);
        return -EINVAL;
    }

    ret = power_supply_get_property(chg_psy,
        POWER_SUPPLY_PROP_USB_TYPE, &chg_usb_type);
    if (ret < 0) {
        GXY_PSY_ERR("%s:get charger/usb_type property fail\n", __func__);
        return ret;
    }

    if (ac_online.intval == 1) {
        if ((s_info->afc_result == 1) ||
            (s_info->chg_pd_type == PD_CONNECT_PE_READY_SNK_PD30)) {
            result = GXY_CHARGING_TYPE_9V_TA;
        } else if (s_info->chg_pd_type == PD_CONNECT_PE_READY_SNK_APDO) {
            result = GXY_CHARGING_TYPE_PDIC_APDO;
        } else {
            result = GXY_CHARGING_TYPE_TA;
        }
    } else if (chg_usb_type.intval == POWER_SUPPLY_USB_TYPE_CDP) {
        result = GXY_CHARGING_TYPE_USB_CDP;
    } else if (chg_usb_type.intval == POWER_SUPPLY_USB_TYPE_SDP) {
        result = GXY_CHARGING_TYPE_USB;
    } else if (chg_usb_type.intval == POWER_SUPPLY_USB_TYPE_UNKNOWN) {
        result = GXY_CHARGING_TYPE_NONE;
    }  else {
        result = GXY_CHARGING_TYPE_UNDEFINED;
    }

    GXY_PSY_ERR("%s: result:%d\n", __func__, result);
    return result;
}
#endif
/*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 end*/
/*Tab A9 code for SR-AX6739A-01-499 by wenyaqi at 20230515 start*/
void gxy_bat_set_afc_result(int afc_result)
{
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
    static struct mtk_charger *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            GXY_PSY_ERR("%s: get mtk-master-charger psy failed\n", __func__);
            return;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                GXY_PSY_ERR("%s: get mtk_charger failed\n", __func__);
                return;
            }
        }
    }

    if (afc_result >= AFC_5V) {
        s_hwinfo_data.afc_result = 1;
        s_info->afc_result = 1;
    } else {
        s_hwinfo_data.afc_result = 0;
        s_info->afc_result = 0;
    }
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
}
EXPORT_SYMBOL(gxy_bat_set_afc_result);

void gxy_bat_set_hv_disable(int hv_disable)
{
    static struct mtk_charger *s_info = NULL;
    struct power_supply *psy = NULL;
    int value = 0;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            pr_err("%s: get mtk-master-charger psy failed\n", __func__);
            return;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                pr_err("%s: get mtk_charger failed\n", __func__);
                return;
            }
        }
    }

    s_info->enable_hv_charging = (hv_disable ? false: true);
    pr_err("%s: set hv_disable = (%d)\n", __func__, value);
}
EXPORT_SYMBOL(gxy_bat_set_hv_disable);
/*Tab A9 code for SR-AX6739A-01-499 by wenyaqi at 20230515 end*/
/*A14 code add for P241227-01958 by chenyulin at 20250106 start*/
void gxy_bat_set_afc_disable(int afc_disable)
{
    static struct mtk_charger *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            pr_err("%s: get mtk-master-charger psy failed\n", __func__);
            return;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                pr_err("%s: get mtk_charger failed\n", __func__);
                return;
            }
        }
    }

    s_info->enable_afc_charging = (afc_disable ? false: true);
    pr_err("%s: set afc_disable = (%d), enable_afc_charging = %d\n", __func__,
           afc_disable, s_info->enable_afc_charging);
}
EXPORT_SYMBOL(gxy_bat_set_afc_disable);
/*A14 code add for P241227-01958 by chenyulin at 20250106 end*/
/*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 start*/
enum hv_charger_status {
    NORMALL_CHARGING_STS = 0,
    FAST_15W_CHARGING_STS,
    FAST_20W_CHARGING_STS,
    FAST_25W_CHARGING_STS,
    FAST_45W_CHARGING_STS,
};

static int ss_fast_charger_status(struct mtk_charger *info)
{
    /*A06_V code for P250126-00952 by yexuedong at 20250211 start*/
    int voltage_vbus_pd = 0;
    int ret = 0;
    struct power_supply *usb_psy = NULL;
    union power_supply_propval prop;
    /*A06_V code for P250126-00952 by yexuedong at 20250211 end*/

    if (info != NULL) {
        if (info->enable_hv_charging == false) {
            return NORMALL_CHARGING_STS;
        }

        if (s_hwinfo_data.afc_result == 1) {
            return FAST_15W_CHARGING_STS;
        }

        if (info->chg_pd_type == PD_CONNECT_PE_READY_SNK ||
            info->chg_pd_type == PD_CONNECT_PE_READY_SNK_PD30 ||
            info->chg_pd_type == PD_CONNECT_PE_READY_SNK_APDO) {
            #if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
            if (info->chg_pd_type == PD_CONNECT_PE_READY_SNK_APDO) {
                return FAST_25W_CHARGING_STS;
            } else
            #endif
            {
                /*A06_V code for P250126-00952 by yexuedong at 20250211 start*/
                usb_psy = power_supply_get_by_name("usb");
                if (usb_psy) {
                    ret = power_supply_get_property(usb_psy,
                    POWER_SUPPLY_PROP_VOLTAGE_NOW, &prop);
                    if (ret < 0) {
                        GXY_PSY_ERR("Couldn't get POWER_SUPPLY_PROP_VOLTAGE_NOW rc=%d\n", ret);
                    } else {
                        voltage_vbus_pd = prop.intval;
                    }
                }
                GXY_PSY_ERR("voltage_now = %d\n", voltage_vbus_pd);
                if (voltage_vbus_pd < 6000) {
                    return NORMALL_CHARGING_STS;
                } else {
                    return FAST_15W_CHARGING_STS;
                }
                /*A06_V code for P250126-00952 by yexuedong at 20250211 end*/
            }
        }
    } else {
        GXY_PSY_ERR("%s:info is NULL\n", __func__);
    }

    return 0;
}
/*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 end*/
/* include attrs for battery psy */
static struct device_attribute gxy_battery_attrs[] = {
    GXY_BATTERY_ATTR(battery_type),
    GXY_BATTERY_ATTR(chg_info),
    GXY_BATTERY_ATTR(tcpc_info),
    GXY_BATTERY_ATTR(batt_cap_control),
    GXY_BATTERY_ATTR(batt_full_capacity),
    GXY_BATTERY_ATTR(store_mode),
    GXY_BATTERY_ATTR(batt_slate_mode),
    GXY_BATTERY_ATTR(input_suspend),
    GXY_BATTERY_ATTR(afc_result),
    GXY_BATTERY_ATTR(hv_disable),
    GXY_BATTERY_ATTR(hv_charger_status),
    GXY_BATTERY_ATTR(shipmode),
    GXY_BATTERY_ATTR(shipmode_reg),
    GXY_BATTERY_ATTR(batt_misc_event),
    GXY_BATTERY_ATTR(batt_current_event),
    GXY_BATTERY_ATTR(batt_temp),
    GXY_BATTERY_ATTR(batt_discharge_level),
    GXY_BATTERY_ATTR(batt_type),
    GXY_BATTERY_ATTR(batt_current_ua_now),
    GXY_BATTERY_ATTR(battery_cycle),
    GXY_BATTERY_ATTR(reset_battery_cycle),
    /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
    GXY_BATTERY_ATTR(battery_cycle_debug),
    /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
    GXY_BATTERY_ATTR(charge_type),
    GXY_BATTERY_ATTR(dump_charger_ic),
    /*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 start*/
    GXY_BATTERY_ATTR(online),
    /*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 end*/
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
    GXY_BATTERY_ATTR(time_to_full_now),
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
    /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
    GXY_BATTERY_ATTR(batt_soc_rechg),
    /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
    /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 start*/
    #if IS_ENABLED(CONFIG_CHARGER_CP_PPS)
    GXY_BATTERY_ATTR(direct_charging_status),
    GXY_BATTERY_ATTR(charging_type),
    #endif
    /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 end*/
    /*A06 code add for AL7160AV-91 by chenyulin at 20250101 start*/
    GXY_BATTERY_ATTR(cp_info),
    /*A06 code add for AL7160AV-91 by chenyulin at 20250101 end*/
};
/* sysfs read for "battery" psd */
ssize_t gxy_bat_show_attrs(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    const ptrdiff_t offset = attr - gxy_battery_attrs;
    int count = 0;
    /*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 start*/
    int ship_flag = 0;
    /*A06_V code for SR-AL7160V-01-164 by yexuedong at 20240910 end*/
    /*A06_V code for SR-AL7160V-01-144 by yexuedong at 20240905 start*/
    int dump_flag = 0;
    /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
    int charge_status = 0;
    /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
    int gxy_time_to_full = 0;
    /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
    /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 start*/
    #if IS_ENABLED(CONFIG_CHARGER_CP_PPS)
    int charging_type = 0;
    #endif
    /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 end*/

    static struct mtk_charger *s_info = NULL;
    struct power_supply *psy = NULL;

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            return -ENODEV;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                return -ENODEV;
            }
        }
    }
    /*A06_V code for SR-AL7160V-01-144 by yexuedong at 20240905 end*/

    switch (offset) {
        case BATTERY_TYPE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    GXY_BATTERY_TYPE_TEXT[s_hwinfo_data.binfo]);
            break;
        /*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 start*/
        case CHG_INFO:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    GXY_BAT_CHG_INFO_TEXT[s_hwinfo_data.cinfo]);
            break;
        /*A06_V code for SR-AL7160V-01-206  by yexuedong at 20240902 end*/
        /*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 start*/
        case TCPC_INFO:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    GXY_BAT_TCPC_INFO_TEXT[s_hwinfo_data.tinfo]);
            break;
        /*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 end*/
        /*A06 code add for AL7160AV-91 by chenyulin at 20250101 start*/
        case CP_INFO:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    GXY_BAT_CP_INFO_TEXT[s_hwinfo_data.pinfo]);
            break;
        /*A06 code add for AL7160AV-91 by chenyulin at 20250101 end*/
        /*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 start*/
        case AFC_RESULT:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    s_hwinfo_data.afc_result);
            break;
        case HV_DISABLE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (s_info->enable_hv_charging ? 0 : 1));
            break;
        /*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 end*/
        case BATT_CAP_CONTROL:
            break;
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
        case BATT_FULL_CAPACITY:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    s_info->gxy_discharge_batt_full_capacity);
            break;
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
        /*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 start*/
        case STORE_MODE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    s_info->gxy_discharge_store_mode);
            break;
        /*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 end*/
        case BATT_SLATE_MODE:
        /*A06_V code for  SR-AL7160V-01-164  by yexuedong at 20240910 start*/
        case INPUT_SUSPEND:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    s_info->gxy_discharge_input_suspend);
            break;
        /*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 start*/
        case HV_CHARGER_STATUS:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (ss_fast_charger_status(s_info)));
            break;
        /*A06_V code for SR-AL7160V-01-152 by xiongxiaoliang at 20240924 end*/
        case SHIPMODE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    s_info->gxy_shipmode_flag);
            break;
        case SHIPMODE_REG:
            ship_flag = s_info->gxy_cint.get_ship_mode(s_info);
            GXY_PSY_INFO("get ship mode reg = %d\n",ship_flag);
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    ship_flag);
            break;
        /*A06_V code for  SR-AL7160V-01-164  by yexuedong at 20240910 end*/
            break;
        /*A06_V code for  SR-AL7160V-01-171 by yexuedong at 20240911 start*/
        case BATT_MISC_EVENT:
        /*A06_V code for SR-AL7160V-01-175 by xiongxiaoliang at 20240920 start*/
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                s_info->gxy_cint.batt_misc_event(s_info));
            break;
        /*A06_V code for  SR-AL7160V-01-171 by yexuedong at 20240911 end*/
        case BATT_CURRENT_EVENT:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                s_info->gxy_cint.batt_current_event(s_info));
            break;
        /*A06_V code for SR-AL7160V-01-175 by xiongxiaoliang at 20240920 end*/
        /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
        case BATT_TEMP:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_bat_get_temp()));
            break;
        case BATT_DISCHARGE_LEVEL:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_get_bat_discharge_level()));
            break;
        case BATT_TYPE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    data_t.cust_batt_type);
            break;
        case BATT_CURRENT_UA_NOW:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_bat_get_current_now()));
            break;
        case BATTERY_CYCLE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_get_bat_cycle()));
            break;
        case RESET_BATTERY_CYCLE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_batt_get_reset_cycle()));
            break;
        case BATTERY_CYCLE_DEBUG:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_get_bat_cycle_debug()));
            break;
        case CHARGE_TYPE:
            charge_status = gxy_get_chr_type();
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    (GXY_CHARGE_TYPE_TEXT[charge_status]));
            break;
        /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
        /*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 start*/
        case DUMP_CHARGER_IC:
            dump_flag = s_info->gxy_cint.dump_charger_ic(s_info);
            GXY_PSY_INFO("dump_charger_ic = %d\n", dump_flag);
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    dump_flag);
            break;
        /*A06_V code for  SR-AL7160V-01-144  by yexuedong at 20240905 end*/
        /*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 start*/
        case ONLINE:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    (gxy_get_batt_online()));
            break;
        /*A06_V code for SR-AL7160V-01-169 by xiongxiaoliang at 20240913 end*/
        /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 start*/
        case TIME_TO_FULL_NOW:
            gxy_time_to_full = s_info->gxy_cint.batt_timetofull();
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    gxy_time_to_full);
            break;
        /*A06_V code for SR-AL7160V-01-161 by xiongxiaoliang at 20240919 end*/
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
        case BATT_SOC_RECHG:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    s_info->gxy_batt_soc_rechg);
            break;
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
        /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 start*/
        #if IS_ENABLED(CONFIG_CHARGER_CP_PPS)
        case DIRECT_CHARGING_STATUS:
            count += scnprintf(buf + count, PAGE_SIZE - count, "%d\n",
                    gxy_get_direct_charging_status());
            break;
        case CHARGING_TYPE:
            charging_type = gxy_get_charging_type();
            count += scnprintf(buf + count, PAGE_SIZE - count, "%s\n",
                    (GXY_CHARGING_TYPE_TEXT[charging_type]));
            break;
        #endif
        /*A06_V code for SR-AL7160V-01-117 by xiongxiaoliang at 20240925 end*/
        default:
            count = -EINVAL;
            break;
    }

    return count;
}

/*A14_V code for P241227-03268 by chenyulin at 20250110 start*/
static int atoi(const char *str)
{
    int result = 0;
    int count = 0;

    if (str == NULL)
        return -EIO;

    while (str[count] != 0	/* NULL */
        && str[count] >= '0' && str[count] <= '9') {
        result = result * 10 + str[count] - '0';
        ++count;
    }

    return result;
}

static int gxy_extract_Before_Space(const char *str) {
    int count = 0;
    char temp[5] = {};
    int value = 0;
    char *space = strchr(str, ' ');

    if (space != NULL) {
        count = space - str;
        strncpy(temp, str, count);
        temp[count] = '\0';
        value = atoi(temp);
        GXY_PSY_INFO("%s: max_soc_string=%s, (%d, %d)\n", __func__, str, count, value);
    } else {
        GXY_PSY_ERR("%s: space is NULL, str= (%s)\n", __func__, str);
    }

    return value;
}
/*A14_V code for P241227-03268 by chenyulin at 20250110 end*/
/* sysfs write for "battery" psd */
ssize_t gxy_bat_store_attrs(
                    struct device *dev,
                    struct device_attribute *attr,
                    const char *buf, size_t count)
{
    const ptrdiff_t offset = attr - gxy_battery_attrs;
    int ret = -EINVAL;
    /*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 start*/
    static struct mtk_charger *s_info = NULL;
    struct power_supply *psy = NULL;
    int value = 0;
    /*A14_V code for P241227-03268 by chenyulin at 20250110 start*/
    int gxy_max_soc = 0;
    /*A14_V code for P241227-03268 by chenyulin at 20250110 start*/

    if (s_info == NULL) {
        psy = power_supply_get_by_name("mtk-master-charger");
        if (psy == NULL) {
            return -ENODEV;
        } else {
            s_info = (struct mtk_charger *)power_supply_get_drvdata(psy);
            if (s_info == NULL) {
                return -ENODEV;
            }
        }
    }

    switch (offset) {
        /*Tab A9 code for SR-AX6739A-01-499 by wenyaqi at 20230515 start*/
        case HV_DISABLE:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                gxy_bat_set_hv_disable(value);
                pr_err("%s: set hv_disable = (%d)\n", __func__, value);
            }
            break;
        /*Tab A9 code for SR-AX6739A-01-499 by wenyaqi at 20230515 end*/
        case BATT_CAP_CONTROL:
            break;
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
        case BATT_FULL_CAPACITY:
             if (!strncmp(buf, gs_basic_protection, (count-1))) {
                s_info->gxy_discharge_batt_full_capacity = GXY_FULL_CAPACITY_LEVEL;
            /*A14_V code for P241227-03268 by chenyulin at 20250110 start*/
            } else if (strstr(buf, gs_maximum_protection)) {
                s_info->gxy_discharge_batt_full_capacity = GXY_MAX_PROTECTION_FLAG;
                gxy_max_soc = gxy_extract_Before_Space(buf);
                if (gxy_max_soc > 0 && gxy_max_soc <= 100) {
                    s_info->gxy_batt_max_protect_soc = gxy_max_soc;
                }
            } else if (!strncmp(buf, gs_sleep_protection, (count-1))) {
                s_info->gxy_discharge_batt_full_capacity = GXY_SLEEP_PROTECTION_FLAG;
            } else if (!strncmp(buf, gs_highsoc_protection, (count-1))) {
                s_info->gxy_discharge_batt_full_capacity = GXY_HIGHSOC_PROTECTION_FLAG;
            } else {
                GXY_PSY_ERR("%s: Setting Value incorrect, cancel batt protection, %s %d\n",
                    __func__, buf, s_info->gxy_discharge_batt_full_capacity);
            }
            GXY_PSY_ERR("%s:set batt protection, type = %s value = %d, max_soc=%d\n", __func__, buf,
                s_info->gxy_discharge_batt_full_capacity, s_info->gxy_batt_max_protect_soc);
            /*A14_V code for P241227-03268 by chenyulin at 20250110 end*/
            ret = count;
            break;
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
        /*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 start*/
        case STORE_MODE:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                s_info->gxy_discharge_store_mode = !!value;
                GXY_PSY_INFO("%s: set store_mode = (%d)\n", __func__, value);
                ret = count;
            }
            break;
        /*A06_V code for SR-AL7160V-01-455 by yexuedong at 20240912 end*/
        /*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 start*/
        case BATT_SLATE_MODE:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                s_info->gxy_discharge_input_suspend = value;
                gxy_batt_set_slate_mode(value);
                ret = count;
            }
            break;
        /*A06_V code for SR-AL7160V-01-178 by yexuedong at 20240918 end*/
        /*A06_V code for SR-AL7160V-01-164  by yexuedong at 20240910 start*/
        case INPUT_SUSPEND:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                s_info->gxy_discharge_input_suspend = value;
                GXY_PSY_INFO("%s: set input_suspend = (%d)\n", __func__, value);
                ret = count;
            }
            break;
        case SHIPMODE:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                s_info->gxy_shipmode_flag = !!value;
                s_info->gxy_cint.set_ship_mode(s_info, s_info->gxy_shipmode_flag);
                GXY_PSY_INFO("%s: enable shipmode val = %d\n",__func__, value);
                ret = count;
            }
            break;
        /*A06_V code for SR-AL7160V-01-164  by yexuedong at 20240910 end*/
        /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 start*/
        case BATT_TYPE:
            strncpy(data_t.cust_batt_type, buf, count);
            data_t.cust_batt_type[count] = '\0';
            ret = count;
            break;
        case RESET_BATTERY_CYCLE:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                gxy_batt_set_reset_cycle(!!value);
                GXY_PSY_INFO("%s: set reset_cycle = (%d)\n", __func__, value);
                ret = count;
            }
            break;
        case BATTERY_CYCLE_DEBUG:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                gxy_set_bat_cycle_debug(value);
                GXY_PSY_INFO("%s: set bat_cycle_debug = (%d)\n", __func__, value);
                ret = count;
            }
            break;
        /*A06_V code for SR-AL7160V-01-162 by xiongxiaoliang at 20240912 end*/
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 start*/
        case BATT_SOC_RECHG:
            if (sscanf(buf, "%10d\n", &value) == 1) {
                s_info->gxy_batt_soc_rechg = !!value;
                ret = count;
            }
            GXY_PSY_ERR("%s:set batt_soc_rechg %d\n", __func__, value);
            break;
        /*A06_V code for SR-AL7160V-01-179 by yexuedong at 20240920 end*/
        default:
            ret = -EINVAL;
            break;
    }

    s_info->gxy_cint.wake_up_charger(s_info);
    /*A06_V code for SR-AL7160V-01-139 by xiongxiaoliang at 20240906 end*/
    return ret;
}

static int gxy_battery_create_attrs(struct device *dev)
{
    unsigned long i = 0;
    int ret = 0;
    for (i = 0; i < ARRAY_SIZE(gxy_battery_attrs); i++) {
        ret = device_create_file(dev, &gxy_battery_attrs[i]);
        if (ret) {
            goto create_attrs_failed;
        }
    }
    goto create_attrs_succeed;
create_attrs_failed:
    pr_err("%s: failed (%d)\n", __func__, ret);
    while (i--) {
        device_remove_file(dev, &gxy_battery_attrs[i]);
    }
create_attrs_succeed:
    return ret;
}

static struct device_attribute gxy_usb_attrs[] = {
    GXY_USB_ATTR(typec_cc_orientation),
};

ssize_t gxy_usb_show_attrs(struct device *dev,
                            struct device_attribute *attr, char * buf)
{
    const ptrdiff_t offset = attr - gxy_usb_attrs;
    int count = 0;

    switch (offset) {
        case TYPEC_CC_ORIENT:
            break;
    default:
        count = -EINVAL;
        break;
    }
    return count;
}

ssize_t gxy_usb_store_attrs(
                    struct device *dev,
                    struct device_attribute *attr,
                    const char *buf, size_t count)
{
    const ptrdiff_t offset = attr - gxy_usb_attrs;
    int ret = -EINVAL;

    switch (offset) {
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}

static int gxy_usb_create_attrs(struct device *dev)
{
    unsigned long i = 0;
    int ret = 0;

    for (i = 0; i < ARRAY_SIZE(gxy_usb_attrs); i++) {
        ret = device_create_file(dev, &gxy_usb_attrs[i]);
        if (ret) {
            goto create_attrs_failed;
        }
    }
    goto create_attrs_succeed;

create_attrs_failed:
    pr_err("%s: failed (%d)\n", __func__, ret);
    while (i--) {
        device_remove_file(dev, &gxy_usb_attrs[i]);
    }

create_attrs_succeed:
    return ret;
}

static int gxy_psy_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct gxy_battery *g_batt = devm_kzalloc(&pdev->dev, sizeof(*g_batt), GFP_KERNEL);

    if (!g_batt) {
        pr_err("%s: dont not kzmalloc\n", __func__);
        return -ENOMEM;
    }

    pr_info("%s: enter\n", __func__);
    g_batt->psy = power_supply_get_by_name("battery");

    if (IS_ERR_OR_NULL(g_batt->psy)) {
        return -EPROBE_DEFER;
    }
    ret = gxy_battery_create_attrs(&g_batt->psy->dev);
    if (ret) {
        pr_err("%s: battery fail to create attrs!!\n", __func__);
        return ret;
    }

    pr_info("%s: Battery Install Success !!\n", __func__);
    g_batt->usb_psy = power_supply_get_by_name("usb");
    if (IS_ERR_OR_NULL(g_batt->usb_psy)) {
        return -EPROBE_DEFER;
    }

    ret = gxy_usb_create_attrs(&g_batt->usb_psy->dev);
    if (ret) {
        pr_err(" %s: usb fail to create attrs!!\n", __func__);
    }

    pr_info("%s: USB Install Success !!\n", __func__);

    return 0;
}

static int gxy_psy_remove(struct platform_device *pdev)
{
    return 0;
}
static const struct of_device_id gxy_psy_of_match[] = {
    {.compatible = "gxy, gxy_psy",},
    {},
};
MODULE_DEVICE_TABLE(of, gxy_psy_of_match);
static struct platform_driver gxy_psy_driver = {
    .probe = gxy_psy_probe,
    .remove = gxy_psy_remove,
    .driver = {
        .name = "gxy_psy",
        .of_match_table = of_match_ptr(gxy_psy_of_match),
    },
};
static int __init gxy_psy_init(void)
{
    pr_info("%s: init\n", __func__);
    return platform_driver_register(&gxy_psy_driver);
}
late_initcall(gxy_psy_init);
static void __exit gxy_psy_exit(void)
{
    platform_driver_unregister(&gxy_psy_driver);
}
module_exit(gxy_psy_exit);
MODULE_AUTHOR("Lucas-Gao");
MODULE_DESCRIPTION("gxy psy driver");
MODULE_LICENSE("GPL");
