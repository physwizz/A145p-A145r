#
# Makefile for the usb FUSB302 driver (if driver is inside kernel tree).
#

ccflags-y += \
		-DFSC_PLATFORM_LINUX \
		-DFSC_HAVE_SNK \
		-DFSC_HAVE_SRC \
		-DFSC_HAVE_DRP \
		-DFSC_HAVE_ACCMODE \
		-DFSC_HAVE_EXT_MSG \
		-DFSC_DEBUG \
		-DFSC_HAVE_VDM  \
		-DFSC_HAVE_ACCMODE
# Set compile option CFLAGS

ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/tcpc/tcpc_fusb302b/Platform_Linux
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/tcpc/tcpc_fusb302b/core
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/tcpc/tcpc_fusb302b/core/modules
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/tcpc/tcpc_fusb302b/core/vdm
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/tcpc/tcpc_fusb302b/core/vdm/DisplayPort

obj-$(CONFIG_TYPEC_FUSB302B) += fusb302_whole.o

fusb302_whole-y := \
		fusb30x_driver.o \
		fusb30x_global.o \
		platform.o \
		platform_helpers.o \
		class-dual-role.o \
		dfs.o \
		../core/core.o \
		../core/PDPolicy.o \
		../core/timer.o \
		../core/fusb30X.o \
		../core/PDProtocol.o \
		../core/TypeC.o \
		../core/Log.o \
		../core/Port.o \
		../core/vendor_info.o \
		../core/modules/dpm.o \
		../core/modules/HostComm.o \
		../core/modules/observer.o \
		../core/vdm/bitfield_translators.o \
		../core/vdm/vdm.o \
		../core/vdm/vdm_callbacks.o \
		../core/vdm/DisplayPort/dp.o \
