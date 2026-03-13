# Makefile for the Himax touchscreen drivers.

ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/ssdev_in_mtk/samsung/


obj-$(CONFIG_ODM_SEC_COMMON_O8) += touch_common_o8.o
touch_common_o8-y += sec_cmd.o
touch_common_o8-y += touch_feature.o
