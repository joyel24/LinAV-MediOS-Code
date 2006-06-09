# Lib makefile 

include ../../../Makefile.conf

ASFLAGS  += -I../../../include
GCCFLAGS += -I../../../include

SRC1 = $(wildcard *.c)
OBJS1 = $(SRC1:%.c=%.o)
SRC2 = $(wildcard *.S)
OBJS2 = $(SRC2:%.S=%.o)
OBJS=$(OBJS1) $(OBJS2)

all: $(OBJS)

clean:    
	@rm -f *.o *.O *.bak *~ 
        
include ../../../default.rule

# End of Makefile
