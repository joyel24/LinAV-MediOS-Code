include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC =  ata.c cpld.c i2c.c rtc.c buttons.c usb_fw.c bat_power.c clkc.c cache.c lcd.c \
    fm_remote_$(FM_REMOTE_VERSION).c fm_remote.c

ifeq ("$(CHIP_TYPE)","DM270")
	SRC += clkc_DM270.c
endif

ifeq ("$(CHIP_TYPE)","DM320")
	SRC += clkc_DM320.c
endif

ifeq ("$(CHIP_TYPE)","DSC21")
	SRC += clkc_DSC21.c
endif

ifeq ("$(CHIP_TYPE)","DSC25")
	SRC += clkc_DSC25.c
endif

ifeq ("$(TARGET_DEVICE)","AV3XX")
	SRC += tsc2003.c
endif

ifeq ("$(TARGET_DEVICE)","AV4XX")
	SRC += tsc2003.c
endif

ifeq ($(HAVE_EXT_MODULE),1)
	SRC += ext_module.c dvr_module.c ir_remote.c cf_module.c
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule