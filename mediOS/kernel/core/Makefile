include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC = dsp.c irq.c malloc.c pipes.c printk.c timer.c trap_fct.c uart.c ti_wdt.c swi.c kdelay.c kernel.c libfloat.c \
    delay.S trap.S thread.c


ifeq ($(HAVE_CMD_LINE),1)
	SRC += cmd_line.c cmd_line_fct.c
endif

ifeq ($(HAVE_EVT),1)
	SRC += evt.c
endif

ifeq ($(HAVE_DBUG),1)
	SRC += bkpt.S
else
	SRC += bkpt_dummy.c
endif

ifeq ($(HAVE_CONSOLE),1)
	SRC += console.c
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule