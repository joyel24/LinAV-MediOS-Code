include ../../Makefile.conf

GCCFLAGS += -I../../include -I../avwm/include

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<