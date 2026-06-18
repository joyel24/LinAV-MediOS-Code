include ../Makefile.conf

GPPFLAGS += -I../include -I./

SRC = $(wildcard *.cpp)
OBJS = $(SRC:%.cpp=%.o)

#SUBDIRS = 

all: $(OBJS)
	$(MV) *.o ../build/

clean: clean-subdirs
	@rm -f *.o *~ 
                        
include ../default.rule