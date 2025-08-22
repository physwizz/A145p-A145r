#ifndef _TOUCH_FEATURE_H_
#define _TOUCH_FEATURE_H_
#include <linux/input.h>
#include <linux/proc_fs.h>
#include <linux/printk.h>
#include <linux/power_supply.h>
#include <linux/syscalls.h>
#include <linux/platform_device.h>
#include <linux/notifier.h>
#include <linux/version.h>
#include <linux/of.h>
#include "sec_cmd.h"

#ifndef CONFIG_ENABLE_REPORT_LOG
#define CONFIG_ENABLE_REPORT_LOG 0
#endif
#define IC_NAME                  "ODM_PROJECT"
#define HWINFO_NAME              "tp_wake_switch"
#define PROC_TPINFO_FILE         "tp_info"
#define TPINFO_STR_LEN           50
#define PANEL_NAME_LEN           80

#define TP_ERROR(fmt, arg...)          pr_err("<%s-ERR>[%s:%d] "fmt"\n", \
                                                   IC_NAME, __func__, __LINE__, ##arg)
#define TP_INFO(fmt, arg...)                \
    do {                                    \
        pr_info("%s-INFO>[%s:%d]"fmt"\n", IC_NAME,__func__, __LINE__, ##arg);\
    } while (0)
#define TP_REPORT(fmt, arg...)                \
    do {                                    \
        if (CONFIG_ENABLE_REPORT_LOG)                        \
            pr_err("<%s-REP>[%s:%d]"fmt"\n", __func__, __LINE__, ##arg);\
    } while (0)

struct lcm_info{
    char* module_name;
    u8 *fw_version;
    struct sec_cmd_data sec;

    void (*gesture_wakeup_enable)(void);
    void (*gesture_wakeup_disable)(void);
    int (*ito_test)(void);
};

struct atag_bootmode {
	u32 size;
	u32 tag;
	u32 bootmode;
	u32 boottype;
};

enum boot_mode_t {
	NORMAL_BOOT = 0,
	META_BOOT = 1,
	RECOVERY_BOOT = 2,
	SW_REBOOT = 3,
	FACTORY_BOOT = 4,
	ADVMETA_BOOT = 5,
	ATE_FACTORY_BOOT = 6,
	ALARM_BOOT = 7,
	KERNEL_POWER_OFF_CHARGING_BOOT = 8,
	LOW_POWER_OFF_CHARGING_BOOT = 9,
	DONGLE_BOOT = 10,
	UNKNOWN_BOOT
};
enum boot_mode_t tp_get_boot_mode(void);

extern  bool g_tp_is_enabled;

int sec_fn_init(struct sec_cmd_data *sec);
void disable_gesture_wakeup(void);
void enable_gesture_wakeup(void);
void disable_high_sensitivity(void);
void enable_high_sensitivity(void); 
bool get_tp_enable(void);
void tp_common_init(struct lcm_info *g_lcm_info);
int tp_detect_panel(const char *tp_ic);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0))
bool tp_choose_panel(const char *lcm_name);
#else//LINUX_VERSION_CODE
const char* tp_choose_panel(void);
#endif//LINUX_VERSION_CODE

/*ear notify*/
int headset_notifier_register(struct notifier_block *nb);
int headset_notifier_unregister(struct notifier_block *nb);
int headset_notifier_call_chain(unsigned long val, void *v);
enum {
    HEADSET_PLUGOUT_STATE=0,
    HEADSET_PLUGIN_STATE=1
};

/*usb notify*/
int register_usb_check_notifier(struct notifier_block *nb);
int unregister_usb_check_notifier(struct notifier_block *nb);
int usb_check_notifier_call_chain(unsigned long val,void *v);
enum {
    TP_USB_PLUGOUT_STATE=3,
    TP_USB_PLUGIN_STATE=4
};



#endif
