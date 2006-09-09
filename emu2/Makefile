include Makefile.conf

GPPFLAGS += -I./include -I./

SRC = $(wildcard *.cpp)
OBJS = $(SRC:%.cpp=%.o)

TARGET = emu

SUBDIRS = emu_core CPU HW cmd_line ext_HW

all: builddir subdirs $(TARGET)

$(TARGET) :
	@echo '-----   building $(TARGET)'
	@$(GPP) $(GPPFLAGS) -o $(TARGET) $(LDFLAGS) build/*.o -lreadline -lncurses -L/usr/X11R6/lib -lX11

clean:  clean-subdirs
	@echo '-----   cleaning MAIN'
	@rm -f *.o *~ *.exe $(TARGET) include/*~ build/*
        
builddir:
	@mkdir -p build
                
include default.rule