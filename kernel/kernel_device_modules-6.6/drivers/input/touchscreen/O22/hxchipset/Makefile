# Makefile for the Himax touchscreen drivers.
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/input/touchscreen/O22/touch_common/

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
ifneq ($(filter m, $(CONFIG_TOUCHSCREEN_HIMAX_COMMON)),)
    ccflags-y += -D__HIMAX_MOD__
endif



obj-$(CONFIG_TOUCHSCREEN_HIMAX_CHIPSET_O22) += himax_ic_HX83112_o22.o
himax_ic_HX83112_o22-y += himax_ic_HX83112.o
himax_ic_HX83112_o22-y += himax_debug.o
himax_ic_HX83112_o22-y += himax_inspection.o
himax_ic_HX83112_o22-y += himax_platform.o
himax_ic_HX83112_o22-y += himax_common.o
himax_ic_HX83112_o22-y += himax_ic_incell_core.o

ifneq ($(filter y, $(CONFIG_TOUCHSCREEN_HIMAX_EMBEDDED_FIRMWARE)),)
    ccflags-y += -DHX_FIRMWARE_HEADER
    himax_ic_HX83112_o22-objs += himax_firmware.o
endif
