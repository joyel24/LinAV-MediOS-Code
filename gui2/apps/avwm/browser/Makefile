include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../include/


SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
	


force: clean all

all: $(OBJS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(OBJS)
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
