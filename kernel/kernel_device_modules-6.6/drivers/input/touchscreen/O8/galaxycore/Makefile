#
# Makefile for the GalaxyCore touchscreen driver.
#
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/input/touchscreen/O8/galaxycore/

obj-$(CONFIG_TOUCHSCREEN_GALAXYCORE_IC_GC7272_O8) += galaxycore_o8.o
galaxycore_o8-y    +=  gcore_drv_main.o
galaxycore_o8-y    +=  gcore_drv_mtk.o
galaxycore_o8-y    +=  gcore_drv_fw_update.o
galaxycore_o8-y    +=  gcore_drv_interface.o
galaxycore_o8-y    +=  gcore_drv_mp_test.o
