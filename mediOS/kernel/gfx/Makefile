include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

SUBDIRS = fonts gui

all: subdirs $(OBJS)
	@cp -f *.o ../build

kernel_lib: all

clean: clean-subdirs 
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule