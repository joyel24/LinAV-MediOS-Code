include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../../avwm/include

ifeq ($(BUILD),X11)
LDFLAGS  += -shared
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)
	@mkdir -p ../../../build/avwm/plugins
	@mv -f *.plug ../../../build/avwm/plugins/

force: clean all

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@.plug  $(LDFLAGS)
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif
	@rm -f *.gdb

clean:
	@rm -f *.o *.bak *~ *.gdb *.plug	
        
include ../../../default.rule
