CROSS = arm-none-eabi-
CC = $(CROSS)gcc
RM = rm
MKDIR = mkdir
BIN2C = bin2c
BASE_DIR = ../../
BUILD_DIR = bin
TARGET_COMMIT = $(shell git log -1 --pretty=format:"%H" | cut -c1-8)
CFLAGS = -Wall -W -pedantic -Wno-main -Wno-long-long -O0 -fpie -march=armv7-a -DTARGET_COMMIT='"$(TARGET_COMMIT)"' -I$(BASE_DIR)/includes -I$(BASE_DIR)/plib -I. -I$(BASE_DIR)/drivers -I$(BASE_DIR)/patchers -I./includes
LDFLAGS = -nostdlib -Tscript.ld -L$(BASE_DIR)/plib -L$(BASE_DIR)/patchers -L$(BASE_DIR)/drivers
LDLIBS = -lpatchers -lp -lgcc -ldrivers
OBJECTS = main.o iboot.o xnu.o jumpto.o
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(OBJECTS))
OBJCOPY = $(CROSS)objcopy

$(BUILD_DIR)/%.o: %.S
	@echo CC -c $(<) -o $(@)
	@$(MKDIR) -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $(<) -o $(@) 

$(BUILD_DIR)/%.o: %.c
	@echo CC -c $(<) -o $(@)
	@$(MKDIR) -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $(<) -o $(@) 

atropine: $(OBJS) | bin/entry.o
	@$(CC) -o $(BUILD_DIR)/payload.elf $(LDFLAGS) $(OBJS) $(LDLIBS)
	@$(OBJCOPY) -O binary $(BUILD_DIR)/payload.elf atropine
	
all: atropine

clean:
	@$(RM) -rf $(BUILD_DIR)
	@$(RM) -rf atropine
