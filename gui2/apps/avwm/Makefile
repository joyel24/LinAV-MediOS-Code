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

force: clean all

all: $(TARGET)
	@mkdir -p ../../build/avwm
	@rm -f ../../build/avwm/avwm
	@mv $(TARGET) ../../build/avwm/
	@cp -f menu.cfg ../../build/avwm/
	@rm -f *.gdb



$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET) include/*~
	@rm -Rf ../../build/avwm/
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
