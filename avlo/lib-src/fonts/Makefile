
TOOL=arm-linux

GCC=$(TOOL)-gcc
LD=$(TOOL)-ld
AR=$(TOOL)-ar
AS=$(TOOL)-as
OBJCOPY= $(TOOL)-objcopy
RANLIB=$(TOOL)-ranlib
RM=rm
CP=cp

TARGET=avLib.a
DEST=../build/lib

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)

GCCFLAGS = -Wa,-R -fno-zero-initialized-in-bss -Wall -I../../include -static-libgcc
ASFLAGS  = -mfpu=softfpa


# State the object files that the main target is made up of
all: $(OBJS)

clean:    
	@rm -f *.o *.O *.bak *~

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<

# DO NOT DELETE
