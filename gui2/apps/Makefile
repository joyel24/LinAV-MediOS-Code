include ../Makefile.conf

GCCFLAGS +=-I../include

LDFLAGS +=-L../lib -lGraphics 

ifeq ($(HAVE_JPEG),y)
LDFLAGS += -ljpeg
endif

ifeq ($(BUILD),X11)
LDFLAGS += -L/usr/X11R6/lib -lX11
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

SUBDIRS = avwm plugins

all: subdirs $(TARGET)
	@mkdir -p ../build/avwm/apps
	@rm -f ../build/avwm/apps/*
	@mv $(TARGET) ../build/avwm/apps
	@rm -f *.gdb

force: clean all

$(TARGET) : $(OBJS)
	@$(GCC) $@.o -o $@  $(LDFLAGS)
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif
	@rm $@.gdb


        
clean: clean-subdirs
	@echo "----- apps dir cleaned"
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET)
	@rm -Rf ../../build/avwm/apps

include ../default.rule
  