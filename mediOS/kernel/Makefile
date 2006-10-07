include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(BUILD_TYPE)","SIMPLE_LOADER")
SUBDIRS = init core lib driver fs target
else
SUBDIRS = api init core lib driver fs snd gfx target
endif

SUBDIRS_CLEAN = build

all: builddir subdirs

medios_lib: SUBDIRS = api
medios_lib: subdirs

builddir:
	@mkdir -p $(BUILD_DIR)
        
include $(MEDIOS_SRC_PATH)/default.rule