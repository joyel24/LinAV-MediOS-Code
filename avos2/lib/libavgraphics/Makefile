# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavgraphics.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include

#LOCAL ASFLAGS
ASFLAGS = -R

# State the object files that the main target is made up of
OBJS = graphics.O graphics1.O\
       graphics2.O graphics4.O graphics8.O graphics16.O\
       graphics32.O osdDSC25.O 


ALL:$(TARGET)

# Rule to make library
$(TARGET): $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)

clean:
	@echo "**** Cleaning..."
	@rm -f include/*~ include/*.bak
	@rm -f *.o *.O *.bak *~
spotless: clean
	@rm -f $(TARGET)
depend:
	makedepend -Y $(GCCINCDIR) *.c
# DO NOT DELETE
