# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = bin2wav

ALL: loader $(TARGET)

loader: loader.o
	@$(OBJCOPY) -O binary loader.o loader.bin
	@cat template.bin.wav loader.bin > 400loader.bin.wav

# Rule to make executable
$(TARGET):
	gcc $(TARGET).c -o $(TARGET)


clean:
	@echo "**** Cleaning..."
	@rm -f *.exe *.o *.bin 400loader.bin.wav *~
	
include ../../default.rule
