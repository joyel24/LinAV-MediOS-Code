include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC = graphics_32.c graphics.c resize.c videnc.c

ifeq ("$(CHIP_TYPE)","DSC21")
SRC += osd_DSC21.c graphics_8_DSC21.c
endif

ifeq ("$(CHIP_TYPE)","DSC25")
SRC += osd_DSC25.c graphics_8_DSC25.c
endif

ifeq ("$(CHIP_TYPE)","DM270")
SRC += osd_DSC25.c graphics_8_DSC25.c
endif

ifeq ("$(CHIP_TYPE)","DM320")
SRC += osd_DSC25.c graphics_8_DSC25.c
endif

SUBDIRS = fonts gui

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule