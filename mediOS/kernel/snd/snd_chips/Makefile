include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC = 

ifeq ($(HAVE_MAS_SOUND),1)
	SRC +=  mas.o
endif

ifeq ($(HAVE_AIC23_SOUND),1)
	SRC += aic23.o
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule