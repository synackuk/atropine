CROSS = arm-none-eabi-
CC = $(CROSS)gcc
RM = rm
BASE_DIR = ./
OBJECTS = display.o
CFLAGS = -Wall -W -pedantic -Wno-long-long -Os -fpie -march=armv7-a -I.


%.o: $(BASE_DIR)%.c
	@echo CC -c $(<) -o $(@)
	@$(CC) $(CFLAGS) -c $(<) -o $(@)

all: $(OBJECTS)

clean:
	@$(RM) -rf *.o