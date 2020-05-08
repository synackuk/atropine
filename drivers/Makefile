CROSS = arm-none-eabi-
CC = $(CROSS)gcc
AR = $(CROSS)ar
RM = rm
OBJS = display/*.o
CFLAGS = -Wall -W -pedantic -Wno-long-long -Os -fpie -march=armv7-a -I.

.PHONY: display
all: libdrivers.a

%.o: $(BASE_DIR)%.c
	@echo CC -c $(<) -o $(@)
	@$(CC) $(CFLAGS) -c $(<) -o $(@)


display:
	@make -C display

libdrivers.a: display drivers.o
	@echo AR crus $(@) $(OBJS) drivers.o
	@$(AR) crus $(@) $(OBJS) drivers.o

clean:
	@make clean -C display
	@$(RM) -rf *.o
	@$(RM) -rf libdrivers.a