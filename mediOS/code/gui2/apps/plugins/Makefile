include ../../Makefile.conf

GCCFLAGS += -I../../include -I../avwm/include

ifeq ($(BUILD),X11)
LDFLAGS  += -shared
endif

LDFLAGS  += -lpthread

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)
SUBDIRS = bluecube test mp3_player clock browser

all: subdirs $(TARGET)
	@mkdir -p ../../build/avwm/plugins
	@rm -f ../../build/avwm/plugins/*
	@mv *.plug ../../build/avwm/plugins/
	@rm -f *.gdb
	@mv -f bluecube/tetris.plug ../../build/avwm/plugins/
	@mv -f mp3_player/mp3_play.plug ../../build/avwm/plugins/play.plug
	@mv -f clock/clock.plug ../../build/avwm/plugins/clock.plug
	@mv -f browser/ls-gui.plug ../../build/avwm/plugins/ls-gui.plug

$(TARGET) : $(OBJS)
	@$(GCC) $@.o -o $@.plug  $(LDFLAGS)   
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif
	@rm -f $@.gdb

force: clean all
                
clean: clean-subdirs
	@echo "----- plugins dir cleaned"
	@rm -f *.o *.bak *~ *.gdb *.plug
	@rm -Rf ../../build/avwm/plugins        

include ../../default.rule
