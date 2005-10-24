include ../../../Makefile.conf

GCCFLAGS+=-I../../../include -I../../../include/kernel

OBJS = $(FONT_LIST:%=%.o)

OPT = $(OBJS:%.o=-D_std%)

all: $(OBJS) font
	@cp -f $(OBJS) ../

font:
	$(GCC) $(GCCFLAGS) -c -o font.o font.c $(OPT)
	@cp -f font.o ../
        
clean: 
	@rm -f *.o *.bak *~ *.gdb

.c.o:
	@echo "[$@]"
	$(GCC) $(GCCFLAGS) -c -o $@ $<