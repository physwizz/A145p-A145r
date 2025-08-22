subdir-ccflags-y += -Wall -Werror -DCONFIG_RT_REGMAP
subdir-ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/power/supply/
subdir-ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/mfd/

obj-$(CONFIG_TCPC_MT6360)		+= tcpc_mt6360.o
obj-$(CONFIG_TCPC_MT6370)		+= tcpc_mt6370.o
obj-$(CONFIG_TCPC_MT6375)		+= tcpc_mt6375.o
obj-$(CONFIG_TCPC_MT6379)		+= tcpc_mt6379.o
obj-$(CONFIG_TCPC_RT1711H)		+= tcpc_rt1711h.o
# A06_V code for SR-AL7160V-01-132|SR-AL7160V-01-136 by xiongxiaoliang at 20240829 start
obj-$(CONFIG_TCPC_CPS8851)		+= tcpc_cps8851.o
# A06 code for SR-AL7160V-01-135 by yexuedong at 20240822 start
obj-$(CONFIG_TCPC_UPM7610)		+= tcpc_upm7610.o
# A06 code for SR-AL7160V-01-135 by yexuedong at 20240822 end
# A06_V code for SR-AL7160V-01-133 by yexuedong at 20240829 start
obj-$(CONFIG_TCPC_UPM6922P)		+= tcpc_upm6922p.o
# A06_V code for SR-AL7160V-01-133 by yexuedong at 20240829 end
obj-$(CONFIG_TCPC_WUSB3812)		+= tcpc_wusb3812.o
# A06_V code for SR-AL7160V-01-132|SR-AL7160V-01-136 by xiongxiaoliang at 20240829 end
# A14_V code for SR-AL6528V-01-117 by gaoxugang at 20240827 start
obj-$(CONFIG_TCPC_HUSB311)      += tcpc_husb311.o
obj-$(CONFIG_HUSB_REGMAP)       += husb-regmap.o
# A14_V code for SR-AL6528V-01-117 by gaoxugang at 20240827 end
# A14 code for SR-AL6528V-01-115 by gaoxugang at 20240911 start
#obj-$(CONFIG_TYPEC_FUSB302B) += tcpc_fusb302b/Platform_Linux/
# A14 code for SR-AL6528V-01-115 by gaoxugang at 20240911 end
# A14 code for SR-AL6528V-01-48 by gaoxugang at 20240929 start
obj-$(CONFIG_TCPC_AW35615)		+= tcpc_aw35615/
# A14 code for SR-AL6528V-01-48 by gaoxugang at 20240929 end
# A14 code for SR-AL6528V-01-119 by gaoxugang at 20240903 start
obj-$(CONFIG_TCPC_ET7304)       += tcpc_et7304.o
obj-$(CONFIG_ET_REGMAP)         += et-regmap.o
# A14 code for SR-AL6528V-01-119 by gaoxugang at 20240903 end

tcpc_class-objs := tcpci_core.o tcpci_typec.o tcpci_timer.o tcpm.o tcpci.o tcpci_alert.o rt-regmap.o

ifdef CONFIG_USB_POWER_DELIVERY
tcpc_class-objs += tcpci_event.o \
		pd_core.o pd_policy_engine.o pd_process_evt.o \
		pd_dpm_core.o pd_dpm_alt_mode_dp.o pd_dpm_pdo_select.o\
		pd_dpm_reaction.o \
		pd_process_evt_snk.o pd_process_evt_src.o pd_process_evt_vdm.o \
		pd_process_evt_drs.o pd_process_evt_prs.o pd_process_evt_vcs.o \
		pd_process_evt_dbg.o pd_process_evt_tcp.o pd_process_evt_com.o \
		pd_policy_engine_src.o pd_policy_engine_snk.o pd_policy_engine_ufp.o pd_policy_engine_vcs.o \
		pd_policy_engine_dfp.o pd_policy_engine_dr.o pd_policy_engine_drs.o pd_policy_engine_prs.o \
		pd_policy_engine_dbg.o pd_policy_engine_com.o
endif
obj-$(CONFIG_TCPC_CLASS) += tcpc_class.o tcpci_late_sync.o pd_dbg_info.o rt_pd_manager.o
