include ../../Makefile.conf

ASFLAGS  += -I./include -I../../include -I. 
#-mfpu=softfpa
GCCFLAGS += -I./include -I../../include -I. -O3

#GCCFLAGS +=  -mno-long-calls

#TOOL=arm-linux
#GCC=$(TOOL)-gcc
#AS=$(TOOL)-as
#LD=$(TOOL)-ld
#OBJCOPY= $(TOOL)-objcopy
#RM=rm
#AR=$(TOOL)-ar
#RANLIB=$(TOOL)-ranlib

TARGET = avboy

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

SUBDIRS =


LDFLAGS =  -nostdlib -Wl, -elf2flt -L/home/Rona/toolchain/gcc-3.3.x/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -L../../ -lmedios 
#-Wl,-Map test.map -Wl,--embedded-relocs -Wl,-s --emit-relocs
# -Wl, -elf2flt -Tapps.lds
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

all: subdirs $(OBJS)
	@cp -f $(OBJS) build
	$(GCC) build/*.o -o $(TARGET).grv  $(LDFLAGS) 
#	$(LD) build/*.o -o $(TARGET).grv $(LDFLAGS)
	@rm -f .elf *.gdb *.elf2flt
        
stdalone: subdirs $(OBJS)
	@cp -f $(OBJS) build
	@$(AR) -rc libavboy.a build/*.o
	@$(RANLIB) libavboy.a

clean:   clean-subdirs
	@rm -f *.o *.bak *~ *.elf *.elf2flt $(TARGET).grv *.gdb

include ../../default.rule