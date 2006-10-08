include $(MEDIOS_SRC_PATH)/Makefile.conf

BUILD_DIR = ../build

SRC = exit.c

ifeq ("$(BUILD_TYPE)","SIMPLE_LOADER")
SRC += boot_loader.S
else
SRC += boot_std.S
endif

all: do_build
        
include $(MEDIOS_SRC_PATH)/default.rule