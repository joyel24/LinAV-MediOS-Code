TOOL=arm-elf

GCC=$(TOOL)-gcc
AR=$(TOOL)-ar
RANLIB=$(TOOL)-ranlib

GCCFLAGS=-I../../include -Iinclude/
LDFLAGS=-elf2flt -L../../lib -lGraphics -ljpeg

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
	
TARGET = avwm

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET) include/*~

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
