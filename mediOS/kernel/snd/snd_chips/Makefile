include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC = 

ifeq ($(HAVE_MAS_SOUND),1)
	SRC +=  mas.c
endif

ifeq ($(HAVE_AIC23_SOUND),1)
	SRC += aic23.c
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule