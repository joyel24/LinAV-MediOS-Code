include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../../avwm/include

ifeq ($(BUILD),X11)
LDFLAGS  += -shared
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)



force: clean all

all: $(TARGET)
	@mkdir -p ../../build/avwm/plugins
	@mv -f $(TARGET) ../../../build/avwm/plugins/

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)	
        

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<