include ../../Makefile.conf

GCCFLAGS += -I../../include -Iinclude/
LDFLAGS += -L../../lib -lGraphics

ifeq ($(HAVE_JPEG),y)
LDFLAGS += -ljpeg
endif

ifeq ($(BUILD),X11)
LDFLAGS += -L/usr/X11R6/lib -lX11
endif

LDFLAGS  += -lpthread -Llib -lavwm

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=%.o)
	
TARGET = avwm

all: $(TARGET)
	@mkdir -p ../../build/avwm
	@rm -f ../../build/avwm/avwm
	@mv $(TARGET) ../../build/avwm/
ifeq ($(BUILD),X11)
	@sed s%\\[CFG_PATH\\]%$(ROOTPATH)%g menu-x11.cfg > ../../build/avwm/menu.cfg
else
	@cp -f menu.cfg ../../build/avwm/menu.cfg
	@cp -Rf icons ../../build/avwm/
endif
	@rm -f *.gdb


force: clean all        
        
$(TARGET) : $(OBJS) browser_dir lib_dir
	$(GCC) $(OBJS) browser/*.o -o $@  $(LDFLAGS)
ifeq ($(HOST),CYGWIN)
	@mv $@.elf.bflt $@
endif
	rm $@.gdb

browser_dir:
	@$(MAKE) -C browser/ all

lib_dir:
	@$(MAKE) -C lib/ all

clean:
	@echo "**** Cleaning..."
	@$(MAKE) -C lib/ clean
	@$(MAKE) -C browser/ clean
	@rm -f *.o *.bak *~ *.gdb *..elf *.elf2flt $(TARGET) include/*~
	@rm -Rf ../../build/avwm/
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
