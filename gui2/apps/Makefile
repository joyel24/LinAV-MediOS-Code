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

force: clean all

all:  plugDir avwmDir $(TARGET)
	@mkdir -p ../build/avwm/apps
	@rm -f ../build/avwm/apps/*
	@mv $(TARGET) ../build/avwm/apps
	@rm -f *.gdb


$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

plugDir:
	@$(MAKE) -C plugins/ all

avwmDir:
	@$(MAKE) -C avwm/ all
	
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET)
	@$(MAKE) -C plugins/ clean
	@$(MAKE) -C avwm/ clean
	@rm -Rf ../../build/avwm/apps
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
