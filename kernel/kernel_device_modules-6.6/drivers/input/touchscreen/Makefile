# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the touchscreen drivers.
#
obj-m += tui-common.o
TOUCH_LISTS := $(subst ",,$(CONFIG_TOUCHSCREEN_MTK_TOUCH))
obj-y += $(foreach TOUCH,$(TOUCH_LISTS),$(TOUCH)/)

ifneq ($(wildcard $(srctree)/$(src)/k6985v1_64_alpha),)
obj-$(CONFIG_TOUCHSCREEN_MT6985_ALPHA_SUPPORT) += k6985v1_64_alpha/
endif

obj-$(CONFIG_TOUCHSCREEN_BOTAI_MULTI_TOUCH) += BoTai_Multi_Touch/
obj-$(CONFIG_TOUCHSCREEN_GT9XX) += gt9xx/
obj-$(CONFIG_TOUCHSCREEN_GOODIX_BRL) += GT9966/
obj-$(CONFIG_TOUCHSCREEN_ILITEK_TOUCH) += ILITEK/
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_CORE_96789) += synaptics_dsx_96789/
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_CORE_96851) += synaptics_dsx_96851/
obj-$(CONFIG_TOUCHSCREEN_FOCALTECH_TOUCH) += focaltech_touch/
obj-$(CONFIG_TOUCHSCREEN_NT36xxx_HOSTDL_SPI) += NT36532/
obj-$(CONFIG_TOUCHSCREEN_GOODIX_9895_BRL) += gt9895_brl/
obj-$(CONFIG_TOUCHSCREEN_SYNA_TCM2) += synaptics_tcm2_sec/
obj-$(CONFIG_TOUCHSCREEN_GOODIX_6936_BRL) += GT6936/
# a06 code for SR-AL7160V-01-62 by wenghailong at 20240826 start
ifeq ($(CONFIG_HQ_PROJECT_O8),y)
obj-$(CONFIG_ODM_SEC_COMMON_O8) += O8/touch_common/
obj-$(CONFIG_TOUCHSCREEN_FT8057P_O8) += O8/focaltech_touch_ft8057p/
obj-$(CONFIG_TOUCHSCREEN_HX83108B_CHIPSET_O8) += O8/hxchipset_hx83108b/
obj-$(CONFIG_TOUCHSCREEN_GALAXYCORE_IC_GC7272_O8) += O8/galaxycore/
endif#CONFIG_HQ_PROJECT_O8
# a06 code for SR-AL7160V-01-62 by wenghailong at 20240826 end
# o22 code for SR-AL6528V-01-137  by wanglei at 202410901 start
ifeq ($(CONFIG_HQ_PROJECT_O22),y)
obj-$(CONFIG_ODM_SEC_COMMON_O22) += O22/touch_common/
obj-$(CONFIG_TOUCHSCREEN_HIMAX_CHIPSET_O22) += O22/hxchipset/
obj-$(CONFIG_TOUCHSCREEN_OMNIVISION_TCM_O22) += O22/omnivision_tcm/
obj-$(CONFIG_TOUCHSCREEN_JADARD_CHIPSET_O22) += O22/jdchipset/
obj-$(CONFIG_TOUCHSCREEN_CHIPONE_TDDI_O22) += O22/chipone-tddi/
endif#CONFIG_HQ_PROJECT_O22
# o22 code for SR-AL6528V-01-137 by wanglei at 20240901 end
