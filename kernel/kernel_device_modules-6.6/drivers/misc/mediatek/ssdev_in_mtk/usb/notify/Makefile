
# usb notify driver
obj-$(CONFIG_USB_HOST_NOTIFY)					+= mod-host-notify-class.o
mod-host-notify-class-$(CONFIG_USB_HOST_NOTIFY) := host_notify_class.o

obj-$(CONFIG_USB_NOTIFY_LAYER)					+= mod-usb-notify-layer.o
mod-usb-notify-layer-$(CONFIG_USB_NOTIFY_LAYER) := usb_notify.o  usb_notify_sysfs.o dock_notify.o

obj-$(CONFIG_USB_NOTIFIER)						+= mod-usb_notifier.o
mod-usb_notifier-$(CONFIG_USB_NOTIFIER)			:= usb_notifier.o

