# Makefile for the Himax touchscreen drivers.

ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/ssdev_in_mtk/samsung/

obj-$(CONFIG_ODM_SEC_COMMON_O22) += touch_common_o22.o
touch_common_o22-y += sec_cmd.o
touch_common_o22-y += touch_feature.o
