include ../../../Makefile.conf

GCCFLAGS += -I../../../include -I../include/

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

TARGET = libavwm.a

all: $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
	@rm -f *.gdb

force: clean all

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(OBJS) $(TARGET)
	        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
