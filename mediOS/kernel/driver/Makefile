include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = ata.o cpld.o i2c.o rtc.o tsc2003.o buttons.o

ifeq ($(HAVE_BAT_POWER),1)
	OBJS += bat_power.o
endif

ifeq ($(HAVE_EXT_MODULE),1)
	OBJS += ext_module.o
endif

ifeq ($(HAVE_FM_REMOTE),1)
	OBJS += fm_remote.o
endif

ifeq ($(HAVE_SOUND),1)
	OBJS += sound.o mas.o
endif

ifeq ($(HAVE_USB_FW),1)
	OBJS += usb_fw.o
endif

ifeq ($(HAVE_DVR),1)
	OBJS += dvr_module.o ir_remote.o
endif

all: $(OBJS)
	@cp -f $(OBJS) ../build

kernel_lib: all

force: clean all        

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule