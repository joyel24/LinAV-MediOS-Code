include ../../../Makefile.conf

ASFLAGS  += -I../../../include
GCCFLAGS += -I../../../include

ifeq ($(HAVE_MAS_SOUND),1)
	OBJS =  mas.o
endif

ifeq ($(HAVE_AIC23_SOUND),1)
	OBJS = aic23.o
endif

all: $(OBJS)
	@cp -f $(OBJS) ../../build

kernel_lib: all

clean: clean-subdirs
	@rm -f *.o *.bak *~ $(TARGET)

include ../../../default.rule