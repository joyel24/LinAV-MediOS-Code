SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

GCC = gcc

all:  $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

plugDir:
	@$(MAKE) -C plugins/ all

avwmDir:
	@$(MAKE) -C avwm/ all
	
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~  $(TARGET)
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
