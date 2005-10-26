include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

all: clean $(OBJS)
	@cp -f $(OBJS) ../build

lib: GCCFLAGS += -DBUILD_LIB
lib:  clean $(OBJS)
                
force: clean all        

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule