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
	@mv $(TARGET) ../../build/avwm/plugins/
	@rm -f *.gdb
	@mv -f bluecube/tetris ../../build/avwm/plugins/
	@mv -f mp3_player/mp3_play ../../build/avwm/plugins/play
	@mv -f clock/clock ../../build/avwm/plugins/clock
	@mv -f browser/ls-gui ../../build/avwm/plugins/ls-gui

$(TARGET) : $(OBJS)
	@$(GCC) $@.o -o $@  $(LDFLAGS)   
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif
	@rm $@.gdb

force: clean all
                
clean: clean-subdirs
	@echo "----- plugins dir cleaned"
	@rm -f *.o *.bak *~ *.gdb $(TARGET)
	@rm -Rf ../../build/avwm/plugins        

include ../../default.rule
