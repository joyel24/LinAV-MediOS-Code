include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../include/

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)


all: $(OBJS)
	@mv *.o ../

force: clean all      

clean:
	@rm -f *~ *.o
        
include ../../../default.rule
