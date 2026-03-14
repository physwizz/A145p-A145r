# SPDX-License-Identifier: GPL-2.0

MTK_PLATFORM := isp4_t
MTK_CUSTOM_PATH := $(DEVICE_MODULES_PATH)/drivers/misc/mediatek
CAM_CAL_SRC_PATH := $(MTK_CUSTOM_PATH)/cam_cal/src
CAM_CAL_PLATFORM_SRC_PATH := $(CAM_CAL_SRC_PATH)/$(MTK_PLATFORM)
COMMON_VERSION := v1
MTK_PLATFORM_KO := camera_eeprom_$(MTK_PLATFORM)
# A06 code for SR-AL7160A by xuyunhui at 20240909 start
#hs14 code for SR-AL6528V-01-230 by zhaoziyu at 2024/09/10 start
#hs14 code for SR-AL6528V-01-231 by jiangwenhan at 2024/09/20 start
CAM_CAL_OBJ := \
	../common/$(COMMON_VERSION)/eeprom_driver.o \
	../common/$(COMMON_VERSION)/eeprom_i2c_common_driver.o \
	eeprom_i2c_custom_driver.o \
	cam_cal_list.o \
	eeprom_i2c_dev.o \
	eeprom_i2c_a0601ddfrontsc820cs_driver.o \
	eeprom_i2c_a0602cxtfrontgc08a8_driver.o \
	eeprom_i2c_a0603ddfrontmt811_driver.o \
	eeprom_i2c_a0604xlfrontmt811_driver.o \
	eeprom_i2c_a0605ddfrontsc820cs_driver.o \
	eeprom_i2c_a0606ddfrontmt811_driver.o \
	eeprom_i2c_a0607xlfrontmt811_driver.o \
	cam_cal_o22_i2c.o \
	eeprom_i2c_a1401widehi556wly_driver.o\
	eeprom_i2c_a1402widesc520cstxd_driver.o\
# A06 code for SR-AL7160A by xuyunhui at 20240909 end
# hs14 code for SR-AL6528V-01-230 by zhaoziyu at 2024/09/10 end
#hs14 code for SR-AL6528V-01-231 by jiangwenhan at 2024/09/20 end
ifeq ($(CONFIG_MTK_CAM_CAL_ISP4_T), m)
obj-$(CONFIG_MTK_CAM_CAL_ISP4_T) += $(MTK_PLATFORM_KO).o
$(eval $(MTK_PLATFORM_KO)-objs += $(CAM_CAL_OBJ))
else ifeq ($(CONFIG_MTK_CAM_CAL_ISP4_T), y)
obj-$(CONFIG_MTK_CAM_CAL_ISP4_T) += $(CAM_CAL_OBJ)
endif

subdir-ccflags-y += -I$(MTK_CUSTOM_PATH)/imgsensor/inc \
                    -I$(MTK_CUSTOM_PATH)/imgsensor/src/$(MTK_PLATFORM)/camera_hw \
                    -I$(MTK_CUSTOM_PATH)/cam_cal/inc \
                    -I$(CAM_CAL_PLATFORM_SRC_PATH) \
                    -I$(CAM_CAL_SRC_PATH)/common/$(COMMON_VERSION) \
                    -I$(srctree)/drivers/i2c/busses/
