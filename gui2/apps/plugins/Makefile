include ../../Makefile.conf

GCCFLAGS += -I../../include -I../avwm/include

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

force: clean all

all: $(TARGET)
	@mkdir -p ../../build/avwm/plugins
	@rm -f ../../build/avwm/plugins/*
	@mv $(TARGET) ../../build/avwm/plugins/
	@rm -f *.gdb
	@$(MAKE) -C bluecube/ all
	@mv -f bluecube/tetris ../../build/avwm/plugins/

$(TARGET) : $(OBJS)
	$(GCC) $@.o -o $@  $(LDFLAGS)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ *.gdb $(TARGET)
	@rm -Rf ../../build/avwm/plugins
	@$(MAKE) -C bluecube/ clean
        

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<