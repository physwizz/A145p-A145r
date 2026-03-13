ifneq ($(CONFIG_MTK_SENSOR_SUPPORT), n)
ifeq ($(subst $(quote),,$(CONFIG_MTK_SENSOR_ARCHITECTURE)), 1.0)
obj-$(CONFIG_CUSTOM_KERNEL_SENSORHUB_1_0)	+= sensorHub/
obj-$(CONFIG_MTK_HWMON)	    += hwmon/
obj-y += sensor_probe/
obj-$(CONFIG_CUSTOM_KERNEL_ALSPS) += alsps/
ifeq ($(CONFIG_CUSTOM_KERNEL_ACCELGYRO),y)
obj-y += accelgyro/
else
obj-$(CONFIG_CUSTOM_KERNEL_ACCELEROMETER) += accelerometer/
obj-$(CONFIG_CUSTOM_KERNEL_GYROSCOPE) += gyroscope/
endif
obj-$(CONFIG_CUSTOM_KERNEL_MAGNETOMETER) += magnetometer/
obj-$(CONFIG_CUSTOM_KERNEL_BAROMETER) += barometer/
obj-$(CONFIG_CUSTOM_KERNEL_STEP_COUNTER)	+= step_counter/
obj-$(CONFIG_CUSTOM_KERNEL_SIGNIFICANT_MOTION_SENSOR)	+= step_counter/
obj-$(CONFIG_CUSTOM_KERNEL_SITUATION) += situation/
obj-$(CONFIG_CUSTOM_KERNEL_SENSOR_FUSION) += sensorfusion/
# a06 code for SR-AL7160V-01-27 by zhawei at 20240823 start
obj-$(CONFIG_CUSTOM_KERNEL_O8_SAR) += o8_sar/
# a06 code for SR-AL7160V-01-27 by zhawei at 20240823 end
# a14 code for SR-AL6528V-01-185 by hefan at 20240827 start
obj-$(CONFIG_CUSTOM_KERNEL_SAR_SENSOR) += o22_sar/
# a14 code for SR-AL6528V-01-185 by hefan at 20240827 end
endif
else
obj-y += dummy.o
endif
