.PHONY: drivers patchers plib targets
all: drivers patchers plib targets

drivers:
	@echo Building $(@)
	@make -C drivers

patchers:
	@echo Building $(@)
	@make -C patchers

targets:
	@echo Building $(@)
	@make -C targets

plib:
	@echo Building $(@)
	@make -C plib

clean:
	@make clean -C targets
	@make clean -C plib
	@make clean -C patchers
	@make clean -C drivers