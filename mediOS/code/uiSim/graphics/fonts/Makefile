GCC=gcc

GCCFLAGS=-I../../include

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

all: $(OBJS)

clean:
	@rm -f *.o *.bak *~ *.gdb

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<