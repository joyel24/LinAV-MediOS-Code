
CC=arm-elf-gcc
CFLAGS=-D_REENTRANT
LDFLAGS=-elf2flt
MICROWINDOWS = ../microwindows-0.90/src


CFLAGS+=-I$(MICROWINDOWS)/include -g
LDFLAGS+=$(MICROWINDOWS)/lib/libnano-X.a $(MICROWINDOWS)/lib/libmwengine.a $(MICROWINDOWS)/lib/libmwdrivers.a $(MICROWINDOWS)/lib/libmwfonts.a

#LDFLAGS+=-L ../jpeg-6b

LDFLAGS += -lpthread 
#-ljpeg

OBJS=\
	example.o \
	snow.o

all: example snow viewer play

example: example.o
	$(CC) example.o -o example  $(CFLAGS) $(LDFLAGS)
	
snow: snow.o
	$(CC) snow.o -o snow  $(CFLAGS) $(LDFLAGS)
	
viewer: viewer.o
	$(CC) viewer.o -o viewer  $(CFLAGS) $(LDFLAGS)
	
play: play.o
	$(CC) play.o -o play  $(CFLAGS) -elf2flt #$(LDFLAGS)

clean: 
	$(RM) $(OBJS) *~ example example.gdb snow snow.gdb viewer viewer.gdb play play.gdb
