include ../../../../Makefile.conf

ASFLAGS  += -I../../../../include
GCCFLAGS += -I../../../../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

all: $(OBJS)
	@cp -f $(OBJS) ../../../build

force: clean all        

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../../../default.rule