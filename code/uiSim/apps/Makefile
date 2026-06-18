
GCC=gcc


GCCFLAGS=-I../include
LDFLAGS=-L../lib -lGraphics -L/usr/X11R6/lib -lX11

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET)
	

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
