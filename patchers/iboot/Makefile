CROSS = arm-none-eabi-
CC = $(CROSS)gcc
RM = rm
BASE_DIR = ./
OBJECTS = finders.o functions.o ibootpatcher.o patchers.o
CFLAGS = -Wall -W -pedantic -Wno-long-long -Wno-multichar -Os -fpie -march=armv7-a -I. -I./includes


%.o: $(BASE_DIR)%.c
	@echo CC -c $(<) -o $(@)
	@$(CC) $(CFLAGS) -c $(<) -o $(@)

all: $(OBJECTS)

clean:
	@$(RM) -rf *.o