include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = delay.o dsp.o irq.o malloc.o pipes.o printk.o timer.o trap_fct.o trap.o uart.o ti_wdt.o 

OBJS += swi.o  bkpt.o kdelay.o kernel.o libfloat.o console.o

ifeq ($(HAVE_CMD_LINE),1)
	OBJS += cmd_line.o
endif

ifeq ($(HAVE_EVT),1)
	OBJS += evt.o
endif

all: $(OBJS)
	@cp -f $(OBJS) ../build

kernel_lib: all

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule