TOOL=arm-elf

GCC=$(TOOL)-gcc
AR=$(TOOL)-ar
RANLIB=$(TOOL)-ranlib

GCCFLAGS=-I../include
LDFLAGS=-elf2flt -L../lib -lGraphics

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : plugDir $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

plugDir:
	@$(MAKE) -C plugins/ all
	
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)
	@$(MAKE) -C plugins/ clean

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
