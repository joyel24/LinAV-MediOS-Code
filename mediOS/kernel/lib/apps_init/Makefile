include ../../../Makefile.conf

ASFLAGS  += -I../../../include
GCCFLAGS += -I../../../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

all: $(OBJS)
	@cp -f ./*.o ../../../lib
clean:  
	@rm -f *.o *.bak *~

include ../../../default.rule