CROSS = arm-none-eabi-
CC = $(CROSS)gcc
RM = rm
BASE_DIR = ./
OBJECTS = display.o
CFLAGS = -Wall -W -pedantic -Wno-long-long -Os -fpie -ffreestanding -funsigned-char -fno-strict-aliasing -std=gnu11 -fcall-used-r9 -march=armv7-a -I.


%.o: $(BASE_DIR)%.c
	@echo CC -c $(<) -o $(@)
	@$(CC) $(CFLAGS) -c $(<) -o $(@)

all: $(OBJECTS)

clean:
	@$(RM) -rf *.o