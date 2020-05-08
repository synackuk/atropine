#include <constants.h>
#include <target.h>

int relocate_init() {
	if(!cmd_ptr_address) {
		return -1;
	}
	cmd_ptr_address->name = "atropine";
	uintptr_t handler = (uintptr_t)cmd_ptr_address->handler + TARGET_LOADADDR_OFFSET;

	cmd_ptr_address->handler = (command_t)handler;
	return 0;
}