include $(MEDIOS_SRC_PATH)/Makefile.conf

SUBDIRS = apps_init
SUBDIRS_CLEAN = $(MEDIOS_SRC_PATH)/lib

all: api_build
                
include $(MEDIOS_SRC_PATH)/default.rule

api_build: lib_dir subdirs $(OBJS)   
	@$(AR) -rc libmedios.a *.o
	@$(RANLIB) libmedios.a
	@mv libmedios.a $(MEDIOS_SRC_PATH)/lib
        
lib_dir:
	@mkdir -p $(MEDIOS_SRC_PATH)/lib
        