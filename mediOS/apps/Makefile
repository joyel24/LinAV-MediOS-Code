include ../Makefile.conf


ASFLAGS  += -I../include

GCCFLAGS += -I../include

LIBS = ../lib/libmedios.a

LDFLAGS = ../lib/*.o -L../lib -lmedios --emit-relocs
all: $(LIBS) othello test apps

othello: othello.o
	@$(LD) -o othello.med othello.o $(LDFLAGS)
#	@$(ELF2FLT) -o othello.grv othello.elf
#	@rm -f .elf *.gdb *.elf2flt

test: test.o
	@$(LD) -o test.med test.o $(LDFLAGS)
#	@$(ELF2FLT) -o test.grv test.elf
#	@rm -f .elf *.gdb *.elf2flt
        
$(LIBS):
	@make -C ../kernel --no-print-directory lib_medios

apps:
	@make -C doom --no-print-directory
	@make -C avboy --no-print-directory
                                
clean:
	@make -C doom --no-print-directory clean
	@make -C avboy --no-print-directory clean
	@rm -f *.o *.bak *~ *.elf *.elf2flt *.grv *.gdb

include ../default.rule