# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the input core drivers.
#
# A06 code for SR-AL7160V-01-78 by hehaoran at 20240827 start
ifeq ($(CONFIG_HQ_PROJECT_O8),y)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/input/touchscreen/O8/touch_common
endif#CONFIG_HQ_PROJECT_O8
# A06 code for SR-AL7160V-01-78 by hehaoran at 20240827 end
# o22 code for SR-AL6528V-01-137  by lihao  at 20240910 start
ifeq ($(CONFIG_HQ_PROJECT_O22),y)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/input/touchscreen/O22/touch_common
endif#CONFIG_HQ_PROJECT_O22
# o22 code for SR-AL6528V-01-137  by lihao at 20240910 end
obj-$(CONFIG_KEYBOARD_MTK_KPD)		+= mtk-kpd.o
obj-$(CONFIG_DEVICE_MODULES_KEYBOARD_MTK_PMIC)		+= mtk-pmic-keys.o
