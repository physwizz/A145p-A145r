# Makefile for the Omnivision TCM touchscreen driver.
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/input/touchscreen/O22/touch_common/

# Each configuration option enables a list of files.

obj-$(CONFIG_TOUCHSCREEN_OMNIVISION_TCM_O22) += omnivision_tcm_o22.o
omnivision_tcm_o22-y += omnivision_tcm_core.o
omnivision_tcm_o22-y += omnivision_tcm_touch.o
omnivision_tcm_o22-y += omnivision_tcm_spi.o
omnivision_tcm_o22-y += omnivision_tcm_zeroflash.o
omnivision_tcm_o22-y += omnivision_tcm_reflash.o
omnivision_tcm_o22-y += omnivision_tcm_device.o
omnivision_tcm_o22-y += omnivision_tcm_recovery.o

ifneq ($(filter y, $(CONFIG_TOUCHSCREEN_OMNIVISION_TCM_DIAGNOSTICS)),)
    omnivision_tcm_o22-y += omnivision_tcm_diagnostics.o
endif

