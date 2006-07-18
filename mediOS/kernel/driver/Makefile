include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = ata.o cpld.o i2c.o rtc.o buttons.o usb_fw.o bat_power.o clkc.o

ifeq ("$(CHIP_TYPE)","DM270")
	OBJS += clkc_DM270.o
endif

ifeq ("$(CHIP_TYPE)","DM320")
	OBJS += clkc_DM270.o
endif

ifeq ("$(CHIP_TYPE)","DSC21")
	OBJS += clkc_DSC21.o
endif

ifeq ("$(CHIP_TYPE)","DSC25")
	OBJS += clkc_DSC25.o
endif


ifeq ($(HAVE_EXT_MODULE),1)
	OBJS += ext_module.o
endif

ifeq ($(HAVE_FM_REMOTE),1)
	OBJS += fm_remote.o
endif

ifeq ($(HAVE_DVR),1)
	OBJS += dvr_module.o ir_remote.o
endif

all: $(OBJS)
	@cp -f $(OBJS) ../build

kernel_lib: all

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule