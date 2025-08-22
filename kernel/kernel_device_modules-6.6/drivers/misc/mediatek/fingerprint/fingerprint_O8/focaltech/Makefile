# Makefile for the focaltech fingerprint driver.
# A06_V code for AL7160AV-521 by yuli at 20250421 start
ccflags-$(CONFIG_DRM_MEDIATEK_V2) += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
# A06_V code for AL7160AV-521 by yuli at 20250421 end
obj-$(CONFIG_FINGERPRINT_FOCALTECH) := focaltech_fp.o
focaltech_fp-objs  := ff_core.o  ff_spi.o
