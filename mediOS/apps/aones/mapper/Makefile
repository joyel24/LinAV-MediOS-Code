include ../../../Makefile.conf

ASFLAGS  += -I../../../include -I. -I../ -I../apu -I../n6502
GCCFLAGS += -I../../../include -I. -I../ -I../apu -I../n6502 -O3

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

all: $(OBJS)
	@cp -f $(OBJS) ../build

clean:  
	@rm -f *.o *.bak *~

include ../../../default.rule