CROSS = arm-none-eabi-
CC = $(CROSS)gcc
RM = rm
BASE_DIR = ./
OBJECTS = finders.o functions.o kernelpatcher.o patchers.o
CFLAGS = -Wall -W -pedantic -Wno-long-long -Os -fpie -fcall-used-r9 -march=armv7-a -I. -I./includes


%.o: $(BASE_DIR)%.c
	@echo CC -c $(<) -o $(@)
	@$(CC) $(CFLAGS) -c $(<) -o $(@)

all: $(OBJECTS)

clean:
	@$(RM) -rf *.o