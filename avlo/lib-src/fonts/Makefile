
include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

FONT_LIST = 6x9 8x13 

OBJS = $(FONT_LIST:%=%.o)

OPT = $(OBJS:%.o=-D_std%)

all: $(OBJS) font
	@cp -f $(OBJS) ../

font:
	$(GCC) $(GCCFLAGS) -c -o font.o font.c $(OPT)
	@cp -f font.o ../
        
clean: 
	@rm -f *.o *.bak *~ *.gdb

include ../../default.rule
