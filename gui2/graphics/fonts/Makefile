TOOL=arm-elf

GCC=$(TOOL)-gcc
AR=$(TOOL)-ar
RANLIB=$(TOOL)-ranlib

GCCFLAGS=-I../../include

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

all: $(OBJS)

clean:
	@rm -f *.o *.bak *~ *.gdb

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<