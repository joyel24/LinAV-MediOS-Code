SRC = $(wildcard *.cpp)
OBJS = $(SRC:%.cpp=%.o)
TARGET = $(SRC:%.cpp=%)

GPP=g++

all: $(TARGET)

$(TARGET) : $(OBJS)
	@$(GPP) $@.o -o $@ 
        
.cpp.o:
	@echo "[$@]"
	@$(GPP) -ggdb3 $(GPPFLAGS) -c -o $@ $<

clean: 
	@rm -f *.o *~ $(TARGET)
