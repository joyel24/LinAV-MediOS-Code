include ../../../../Makefile.conf

ASFLAGS  += -I../../../../include
GCCFLAGS += -I../../../../include

SUBDIRS = gui

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

all: subdirs $(OBJS)
	@cp -f $(OBJS) ../build

force: clean all        

kernel_lib: all

clean: clean-subdirs 
	@rm -f *.o *.bak *~ $(TARGET)

include ../../../../default.rule