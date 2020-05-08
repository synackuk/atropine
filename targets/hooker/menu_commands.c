#include <plib.h>
#include <constants.h>
#include <command.h>
#include <image.h>
#include <command.h>

static int go_command(int argc, command_args* argv) {
	char* addr = (char*)load_address;
	load_image(addr);
	jumpto(0, addr, NULL);
	return 0;
}

static int load_atropine_command(int argc, command_args* argv) {
	size_t atropine_len = get_env_uint("filesize");
	uint32_t* atropine = (uint32_t*)((uintptr_t)0x10000000 + 0x70000);
	memcpy(atropine, load_address, atropine_len);
	return 0;
}

int menu_commands_init() {
	add_command("load-atropine", &load_atropine_command, NULL);
	add_command("go", &go_command, NULL);
	return 0;
}