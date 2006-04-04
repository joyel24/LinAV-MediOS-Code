include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = ata.o cpld.o i2c.o rtc.o buttons.o usb_fw.o bat_power.o

ifeq ($(HAVE_EXT_MODULE),1)
	OBJS += ext_module.o
endif

ifeq ($(HAVE_FM_REMOTE),1)
	OBJS += fm_remote.o
endif

ifeq ($(HAVE_MAS_SOUND),1)
	OBJS += sound.o mas.o
endif

ifeq ($(HAVE_AIC23_SOUND),1)
	OBJS += aic23.o
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