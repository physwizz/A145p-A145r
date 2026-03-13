# Makefile for the focaltech fingerprint driver.
# A14_V code for AL6528AV-456 by yuli at 20250421 start
ccflags-$(CONFIG_DRM_MEDIATEK_V2) += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
# A14_V code for AL6528AV-456 by yuli at 20250421 end
obj-$(CONFIG_FINGERPRINT_FOCALTECH) := focaltech_fp.o
focaltech_fp-objs  := ff_core.o  ff_spi.o
