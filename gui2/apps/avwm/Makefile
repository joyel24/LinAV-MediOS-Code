include ../../Makefile.conf

GCCFLAGS += -I../../include -Iinclude/
LDFLAGS += -L../../lib -lGraphics

ifeq ($(HAVE_JPEG),y)
LDFLAGS += -ljpeg
endif

ifeq ($(BUILD),X11)
LDFLAGS += -L/usr/X11R6/lib -lX11
endif

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
	
TARGET = avwm

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET) include/*~

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
