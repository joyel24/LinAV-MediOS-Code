include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(BUILD_TYPE)","SIMPLE_LOADER")
SUBDIRS = core driver init
else
SUBDIRS = core driver gfx init
endif

#fs

SUBDIRS_CLEAN = build

all: builddir subdirs 

builddir:
	@mkdir -p build
        
include $(MEDIOS_SRC_PATH)/default.rule