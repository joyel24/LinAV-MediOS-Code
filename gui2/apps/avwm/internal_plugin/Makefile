include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../include/

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)


all: $(OBJS)

force: clean all      

clean:
	@rm -f *~ *.o
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o ../$@ $<
