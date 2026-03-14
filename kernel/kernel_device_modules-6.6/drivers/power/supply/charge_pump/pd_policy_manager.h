#ifndef __PD_POLICY_MANAGER_H__
#define __PD_POLICY_MANAGER_H__

struct gxy_cp_ops {
    int (*get_present)(int *value);
    int (*set_present)(int *value);
    int (*set_charging_enabled)(bool *value);
    int (*get_charging_enabled)(bool *value);
    int (*get_vbus)(int *value);
    int (*get_ibus)(int *value);
    int (*get_vbat)(int *value);
    int (*get_ibat)(int *value);
    int (*get_temp)(int *value);
    /*A06_V code for SR-AL7160V-01-92 by xiongxiaoliang at 20240904 start*/
    int (*set_otg_txmode)(int *value);
    int (*get_otg_txmode)(int *value);
    /*A06_V code for SR-AL7160V-01-92 by xiongxiaoliang at 20240904 end*/
};

/*A06_V code for SR-AL7160V-01-135 by xiongxiaoliang at 20240902 start*/
struct gxy_cp_info {
    int cp_chg_enable;
    int cp_jeita_stop_chg;
    int cp_thermal_stop_chg;
    int cp_fac_stop_chg;
    int cp_jeita_cc;
    int cp_jeita_cv;
    int cp_battery_cv;
    int cp_therm_cc;
    bool cp_ss_stop_chg;
};
/*A06_V code for SR-AL7160V-01-135 by xiongxiaoliang at 20240902 end*/

//extern struct gxy_cp_info g_gxy_cp_info;

#endif /* SRC_PDLIB_USB_PD_POLICY_MANAGER_H_ */
