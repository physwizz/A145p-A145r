#
# Makefile for muic devices
#

obj-$(CONFIG_MUIC_CORE)		+= mod-core-muic.o
mod-core-muic-$(CONFIG_MUIC_CORE)		:= muic_core.o
mod-core-muic-$(CONFIG_MUIC_MANAGER)	+= muic_manager.o
mod-core-muic-$(CONFIG_MUIC_SYSFS)		+= muic_sysfs.o

obj-$(CONFIG_MUIC_SM5714)		+= mod-sm5714-muic.o
mod-sm5714-muic-$(CONFIG_MUIC_SM5714)		:= sm5714-muic.o sm5714-muic-afc.o
mod-sm5714-muic-$(CONFIG_MUIC_SUPPORT_PDIC) += sm5714-muic-pdic.o

obj-$(CONFIG_MUIC_S2MU106)		+= mod-s2mu106-muic.o
mod-s2mu106-muic-$(CONFIG_MUIC_S2MU106)			:= s2mu106-muic.o
mod-s2mu106-muic-$(CONFIG_HV_MUIC_S2MU106_AFC)	+= s2mu106-muic-afc.o

obj-$(CONFIG_MUIC_S2MU107)		+= mod-s2mu107-muic.o
mod-s2mu107-muic-$(CONFIG_MUIC_S2MU107)			:= s2mu107-muic.o
mod-s2mu107-muic--$(CONFIG_HV_MUIC_S2MU107_AFC)	+= s2mu107-muic-afc.o

obj-$(CONFIG_MUIC_NOTIFIER)		+= mod-muic-notifier.o
mod-muic-notifier-$(CONFIG_MUIC_NOTIFIER)		:= muic_notifier.o