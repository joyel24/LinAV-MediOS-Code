include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../include/


SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

all: $(OBJS)

force: clean all

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(OBJS)
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
