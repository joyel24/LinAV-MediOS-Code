include ../../Makefile.conf

GPPFLAGS += -I../../include -I./

SRC = $(wildcard *.cpp)
OBJS = $(SRC:%.cpp=%.o)


all: $(OBJS)
	$(MV) *.o ../../build/

clean:
	@rm -f *.o *~ 
                        
include ../../default.rule