include ../Makefile.conf

GCCFLAGS +=-I../include

LDFLAGS +=-L../lib -lGraphics 

ifeq ($(HAVE_JPEG),y)
LDFLAGS += -ljpeg
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : plugDir avwmDir $(OBJS)
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

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
