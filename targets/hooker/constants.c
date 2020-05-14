#include <addresses.h>
#include <constants.h>
#include <finders.h>

command_descriptor* cmd_ptr_address;
uintptr_t* base_address;
uintptr_t* load_address;
malloc_t _malloc;
get_env_t _get_env;
get_env_uint_t _get_env_uint;
uintptr_t* framebuffer_address;
uint32_t display_width;
uint32_t display_height;
void* image_list;
int version;
set_env_uint_t _set_env_uint;

int constants_init() {
	base_address = BASE_ADDRESS;
	if(!base_address) {
		return -1;
	}	
	_malloc = MALLOC_ADDRESS;
	if(!_malloc) {
		return -1;
	}
	cmd_ptr_address = (command_descriptor*)CMD_PTR_ADDRESS;
	if(!cmd_ptr_address) {
		return -1;
	}
	_get_env = GET_ENV_ADDRESS;
	if(!_get_env) {
		return -1;
	}
	_get_env_uint = GET_ENV_UINT_ADDRESS;
	if(!_get_env_uint) {
		return -1;
	}
	load_address = LOAD_ADDRESS;
	if(!load_address) {
		return -1;
	}
	framebuffer_address = FRAMEBUFFER_ADDRESS;
	if(!framebuffer_address) {
		return -1;
	}
	display_width = DISPLAY_WIDTH;
	if(!display_width) {
		return -1;
	}
	display_height = DISPLAY_HEIGHT;
	if(!display_height) {
		return -1;
	}
	image_list = IMAGE_LIST_ADDRESS;
	if(!image_list) {
		return -1;
	}

	version = VERSION;
	if(!version) {
		return -1;
	}

	_set_env_uint = SET_ENV_UINT_ADDRESS;
	if(!_set_env_uint) {
		return -1;
	}
	return 0;
}