TOOL=arm-elf

GCC=$(TOOL)-gcc
AR=$(TOOL)-ar
RANLIB=$(TOOL)-ranlib

GCCFLAGS=-I../../include
LDFLAGS=-elf2flt

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<