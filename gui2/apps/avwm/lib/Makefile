include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../include/

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

TARGET = libavwm.a

force: clean all

all: $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
	@rm -f *.gdb

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(OBJS) $(TARGET)
	        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
