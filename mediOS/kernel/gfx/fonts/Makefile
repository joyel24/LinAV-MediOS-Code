include ../../../Makefile.conf

GCCFLAGS+=-I../../../include -I../../../include/kernel

@SRC = $(wildcard *.c)
@OBJS = $(SRC:%.c=%.o)
OBJS = 4x6.o  6x9.o 8x13.o

OPT = $(OBJS:%.o=-D_std%)

all: $(OBJS) font
	@cp -f $(OBJS) ../

font:
	$(GCC) $(GCCFLAGS) -c -o font.o font.c $(OPT)
	@cp -f font.o ../
        
clean: 
	@rm -f *.o *.bak *~ *.gdb

.c.o:
	@echo "[$@]"
	$(GCC) $(GCCFLAGS) -c -o $@ $<