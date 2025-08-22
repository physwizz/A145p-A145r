#obj-y += a1403backhi5022qly_mipi_raw.o
IMGSENSOR_SUBDRV_OBJ := \
	$(subdrv-rpath)/a1403backhi5022qly_mipi_raw.o \

ifeq ($(CONFIG_MTK_IMGSENSOR_ISP4_T), m)
$(eval $(MTK_PLATFORM_KO)-objs += $(IMGSENSOR_SUBDRV_OBJ))
else ifeq ($(CONFIG_MTK_IMGSENSOR_ISP4_T), y)
obj-$(CONFIG_MTK_IMGSENSOR_ISP4_T) += $(IMGSENSOR_SUBDRV_OBJ)
endif