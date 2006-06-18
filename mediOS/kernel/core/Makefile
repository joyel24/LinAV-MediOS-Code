include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = delay.o dsp.o irq.o malloc.o pipes.o printk.o timer.o trap_fct.o trap.o uart.o ti_wdt.o 

OBJS += swi.o kdelay.o kernel.o libfloat.o

ifeq ($(HAVE_CMD_LINE),1)
	OBJS += cmd_line.o
endif

ifeq ($(HAVE_EVT),1)
	OBJS += evt.o
endif

ifeq ($(HAVE_DBUG),1)
	OBJS += bkpt.o
else
	OBJS += bkpt_dummy.o
endif

ifeq ($(HAVE_CONSOLE),1)
	OBJS += console.o
endif

all: $(OBJS)
	@cp -f $(OBJS) ../build

kernel_lib: all

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule