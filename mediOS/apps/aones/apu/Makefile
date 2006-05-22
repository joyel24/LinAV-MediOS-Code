include ../../../Makefile.conf

ASFLAGS  += -I../../../include -I. -I../ 
GCCFLAGS += -I../../../include -I. -I../

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

all: $(OBJS)
	@cp -f $(OBJS) ../build

clean:  
	@rm -f *.o *.bak *~

include ../../../default.rule