include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../../avwm/include

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = tetris

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