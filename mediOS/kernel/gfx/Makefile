include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = graphics_32.o graphics.o resize.o

ifeq ("$(CHIP_TYPE)","DSC21")
OBJS += osd_DSC21.o graphics_8_DSC21.o
endif

ifeq ("$(CHIP_TYPE)","DSC25")
OBJS += osd_DSC25.o graphics_8_DSC25.o
endif

ifeq ("$(CHIP_TYPE)","DM270")
OBJS += osd_DSC25.o graphics_8_DSC25.o
endif

ifeq ("$(CHIP_TYPE)","DM320")
OBJS += osd_DSC25.o graphics_8_DSC25.o
endif


SUBDIRS = fonts gui

all: subdirs $(OBJS)
	@cp -f *.o ../build

kernel_lib: all

clean: clean-subdirs 
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule