include ../../../Makefile.conf

ASFLAGS  += -I../../../include
GCCFLAGS += -I../../../include

OBJS = apps_start.o apps_stop.o 

all: $(OBJS)
	@cp -f ./*.o ../../../lib
clean:  
	@rm -f *.o *.bak *~

include ../../../default.rule