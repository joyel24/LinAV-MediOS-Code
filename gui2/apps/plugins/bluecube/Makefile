include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../../avwm/include

ifeq ($(BUILD),X11)
LDFLAGS  += -shared
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = tetris

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $@.plug  $(LDFLAGS)
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif        
	@rm -f *.gdb

clean:
	@rm -f *.o *.bak *~ *.gdb *.plug

include ../../../default.rule
