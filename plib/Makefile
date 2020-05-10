CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AR = $(CROSS)ar
RM = rm
BASE_DIR = ./
BUILD_DIR = bin
OBJECTS = atoi.o div.o memcmp.o memmem.o memmove.o memset.o strcmp.o strncmp.o strcpy.o strncat.o strcat.o strlen.o ultoa.o printf.o panic.o putc.o cache.o cpu.o
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(OBJECTS))
CFLAGS = -Wall -W -pedantic -Wno-long-long -Os -fpie -fcall-used-r9 -march=armv7-a -I.

$(BUILD_DIR)/%.o: $(BASE_DIR)%.c
	@echo CC -c $(<) -o $(@)
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $(<) -o $(@)

$(BUILD_DIR)/%.o: $(BASE_DIR)%.S
	@echo CC -c $(<) -o $(@)
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $(<) -o $(@) 

libp.a: $(OBJS)
	@echo AR crus $(@) $(OBJS)
	@$(AR) crus $(@) $(OBJS)
all: libp.a

clean:
	@$(RM) -rf $(BUILD_DIR)
	@$(RM) -rf libp.a