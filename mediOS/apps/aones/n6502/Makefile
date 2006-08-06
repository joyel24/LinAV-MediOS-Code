include ../../../Makefile.conf

ASFLAGS  += -I../../../include -I. -I../
GCCFLAGS += -I../../../include -I. -I../ -O3

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o) ncpu.o

all: $(OBJS)
	@cp -f $(OBJS) ../build

ncpu.o:
	@echo "[$@]"
	@$(GCC) $(GCCFLAGS) -c ncpu.S -o ncpu.o

include ../../../default.rule