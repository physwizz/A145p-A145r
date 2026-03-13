ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/input/touchscreen/O22/touch_common/

MTK_PLATFORM := $(subst ",,$(CONFIG_MTK_PLATFORM))
subdir-ccflags-y += -Werror -I$(srctree)/drivers/misc/mediatek/include
subdir-ccflags-y += -Werror -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include
subdir-ccflags-y += -Werror -I$(srctree)/drivers/misc/mediatek/include/mt-plat
subdir-ccflags-y += -I$(srctree)/drivers/power/supply/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/eccci/inc/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/extcon/

obj-$(CONFIG_TOUCHSCREEN_CHIPONE_TDDI_O22) += chipone-tddi_o22.o
chipone-tddi_o22-objs := cts_i2c_driver.o
chipone-tddi_o22-objs += cts_core.o
chipone-tddi_o22-objs += cts_sfctrlv2.o
chipone-tddi_o22-objs += cts_spi_flash.o
chipone-tddi_o22-objs += cts_firmware.o
chipone-tddi_o22-objs += cts_charger_detect.o
chipone-tddi_o22-objs += cts_earjack_detect.o
chipone-tddi_o22-objs += cts_hostcomm.o
chipone-tddi_o22-objs += cts_tcs.o
chipone-tddi_o22-objs += cts_platform.o
chipone-tddi_o22-objs += cts_tool.o
chipone-tddi_o22-objs += cts_sysfs.o
chipone-tddi_o22-objs += cts_strerror.o
chipone-tddi_o22-objs += cts_oem.o
