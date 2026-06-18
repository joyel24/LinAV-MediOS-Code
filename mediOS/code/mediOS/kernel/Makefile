include $(MEDIOS_SRC_PATH)/Makefile.conf

SUBDIRS = api init core lib driver fs snd gfx target

SUBDIRS_CLEAN = build

all: builddir subdirs

medios_lib: SUBDIRS = api
medios_lib: subdirs

builddir:
	@mkdir -p $(BUILD_DIR)
        
include $(MEDIOS_SRC_PATH)/default.rule