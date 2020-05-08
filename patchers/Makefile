CROSS = arm-none-eabi-
AR = $(CROSS)ar
RM = rm
OBJS = xnu/*.o iboot/*.o

.PHONY: xnu iboot
all: libpatchers.a

xnu:
	@make -C xnu

iboot:
	@make -C iboot

libpatchers.a: xnu iboot
	@echo AR crus $(@) $(OBJS)
	@$(AR) crus $(@) $(OBJS)

clean:
	@make clean -C xnu
	@make clean -C iboot
	@$(RM) -rf libpatchers.a