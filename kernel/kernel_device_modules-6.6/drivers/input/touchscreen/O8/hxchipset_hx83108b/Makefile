# Makefile for the Himax touchscreen drivers.

ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/

ifneq ($(filter y, $(CONFIG_KALLSYMS_ALL)),)
    ccflags-y += -D__KERNEL_KALLSYMS_ALL_ENABLED__
endif

ifneq ($(filter y, $(CONFIG_FB)),)
    ccflags-y += -DHX_CONFIG_FB
endif

ifneq ($(filter y, $(CONFIG_DRM)),)
    ccflags-y += -DHX_CONFIG_DRM
endif

ifneq ($(filter y, $(CONFIG_PM)),)
    ccflags-y += -DHX_CONFIG_PM
endif

obj-$(CONFIG_TOUCHSCREEN_HX83108B_CHIPSET_O8) += hxchipset_hx83108b_o8.o

hxchipset_hx83108b_o8-y += himax_debug.o
hxchipset_hx83108b_o8-y += himax_inspection.o
hxchipset_hx83108b_o8-y += himax_ic_HX83108a.o
hxchipset_hx83108b_o8-y += himax_ic_core.o
hxchipset_hx83108b_o8-y += himax_common.o
hxchipset_hx83108b_o8-y += himax_platform.o

