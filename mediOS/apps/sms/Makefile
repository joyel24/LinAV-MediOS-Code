include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -Icpu
#-mfpu=softfpa
GCCFLAGS += -I../../include -I. -Icpu

#GCCFLAGS +=  -mno-long-calls

#TOOL=arm-linux
#GCC=$(TOOL)-gcc
#AS=$(TOOL)-as
#LD=$(TOOL)-ld
#OBJCOPY= $(TOOL)-objcopy
#RM=rm
#AR=$(TOOL)-ar
#RANLIB=$(TOOL)-ranlib

TARGET = av_sms

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

SUBDIRS = cpu sound


LDFLAGS =  -nostdlib -Wl, -elf2flt -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -L../../ -lmedios 
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
	@$(AR) -rc libsms.a build/*.o
	@$(RANLIB) libsms.a

clean:   clean-subdirs
	@rm -f *.o *.bak *~ *.elf *.elf2flt $(TARGET).grv *.gdb

include ../../default.rule