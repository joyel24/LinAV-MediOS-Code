include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

LDFLAGS = -L../../ -lgravity  -Wl, -elf2flt -lgcc
#-Wl, -elf2flt
TARGET = av_sms

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o) cpu/z80.o sound/emu2413.o sound/sn76496.o

all: builddir $(OBJS)
	$(GCC) $(OBJS) -o $(TARGET).grv  $(LDFLAGS) $(GCCFLAGS)
	@rm -f *.gdb

force: clean all        

clean:  
	@rm -f *.o *.bak *~ $(TARGET) cpu/*.o sound/*.o

builddir:
	@mkdir -p build
        
include ../../default.rule
