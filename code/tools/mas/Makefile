SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

GCC = gcc

all:  $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  -I./
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~  $(TARGET)
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
