include ../../../Makefile.conf


ASFLAGS  += -I../../../include -I. -I../
#-mfpu=softfpa
GCCFLAGS += -I../../../include -I. -I../

#GCCFLAGS +=  -mno-long-calls

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


#-Wl,-Map test.map -Wl,--embedded-relocs -Wl,-s --emit-relocs
# -Wl, -elf2flt
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

all: $(OBJS)
	@cp -f $(OBJS) ../build
clean:  
	@rm -f *.o *.bak *~

include ../../../default.rule