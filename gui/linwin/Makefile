#
# use 'make IPOD=1' to build for iPod, or 'make' for host
# 

CC=arm-elf-gcc
CFLAGS=-DIPOD -D_REENTRANT
LDFLAGS=-elf2flt
MICROWINDOWS = ../microwindows-0.90/src

# enable JPEG support
#LDFLAGS+=-L ../jpeg-6b

CFLAGS+=-I$(MICROWINDOWS)/include -g
LDFLAGS+=$(MICROWINDOWS)/lib/libnano-X.a $(MICROWINDOWS)/lib/libmwengine.a $(MICROWINDOWS)/lib/libmwdrivers.a $(MICROWINDOWS)/lib/libmwfonts.a

# enable JPEG support
#LDFLAGS += -ljpeg

LDFLAGS += -lpthread

OBJS=\
	pz.o \
	display.o \
	image.o \
	browser.o \
	ipod.o \
	menu.o \
	piezo.o \
	textview.o \
	mp3.o \
	message.o \
	slider.o \
	audio.o \
	calendar.o \
	othello.o 

all: linwin

linwin: $(OBJS)
	$(CC) $(OBJS) -o linwin  $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(OBJS) *~ linwin linwin.gdb
