include Makefile.conf

GPPFLAGS += -I./include -I./

SRC = $(wildcard *.cpp)
OBJS = $(SRC:%.cpp=%.o)

TARGET = emu

SUBDIRS = CPU HW cmd_line

all: builddir subdirs $(TARGET)

$(TARGET) : $(OBJS)
	@echo '-----   building $(TARGET)'
	@$(MV) *.o build/
	@$(GPP) -o $(TARGET) -ggdb3 -lreadline -lncurses $(LDFLAGS) build/*.o 

clean:  clean-subdirs
	@echo '-----   cleaning MAIN'
	@rm -f *.o *~ $(TARGET) include/*~ 
        
builddir:
	@mkdir -p build
                
include default.rule