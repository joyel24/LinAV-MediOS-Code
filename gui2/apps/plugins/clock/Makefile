include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../../avwm/include

ifeq ($(BUILD),X11)
LDFLAGS  += -shared
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = clock

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $@  $(LDFLAGS)
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif
	@rm -f *.gdb

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<