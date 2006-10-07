include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(BUILD_TYPE)","SIMPLE_LOADER")
SRC = ata.c cpld.c
else
SRC = ata.c buttons.c cpld.c
endif

BUILD_DIR = ../build

all: do_build
        
include $(MEDIOS_SRC_PATH)/default.rule