# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = bin2avi

ALL: $(TARGET) loader

loader:
	arm-linux-as 402loader.S -o 402loader.o
	arm-linux-uclibc-objcopy -O binary 402loader.o 402loader.bin
	cat template.avi 402loader.bin > 402loader.bin.avi

	
# Rule to make executable
$(TARGET):
	gcc $(TARGET).c -o $(TARGET)


clean:
	@echo "**** Cleaning..."
	@rm -f *.exe *.o *.bin *.elf 402loader.bin.avi 402temp.avi 402temp2.avi *~
	
include ../../default.rule
