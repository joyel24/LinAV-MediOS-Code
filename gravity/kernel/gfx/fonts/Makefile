include ../../Makefile.conf

GCCFLAGS=-I../../../include -I../../../include/kernel

@SRC = $(wildcard *.c)
@OBJS = $(SRC:%.c=%.o)
OBJS = 4x6.o

all: $(OBJS)
	@cp -f $(OBJS) ../

clean: 
	@rm -f *.o *.bak *~ *.gdb

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<