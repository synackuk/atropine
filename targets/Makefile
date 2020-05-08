.PHONY: payload hooker

all: payload hooker

payload: 
	@echo Building target $(@)
	@make -C payload

hooker: 
	@echo Building target $(@)
	@make -C hooker

clean:
	@make clean -C payload
	@make clean -C hooker