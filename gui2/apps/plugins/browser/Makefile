include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../../avwm/include

ifeq ($(BUILD),X11)
LDFLAGS  += -shared
endif

LDFLAGS  += -L../../avwm/lib -lavwm -lpthread

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = ls-gui

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $@  $(LDFLAGS)
	@rm -f *.gdb

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<