#include <plib.h>
#include <constants.h>
#include <command.h>
#include <image.h>

static int load_command(int argc, command_args* argv) {
	char* addr = (char*)load_address;
	unsigned int size;
	if(argc != 2) {
		return -1;
	}
	if(!strcmp(argv[1].string, "ibot")) {
		load_image_from_bdev(addr, IBOOT_TAG, (size_t*)&size);
	}
	set_env_uint("filesize", size, 0);
	return 0;
}

static int load_payload_command(int argc, command_args* argv) {
	size_t payload_len = get_env_uint("filesize");
	uint32_t* payload = (uint32_t*)((uintptr_t)0x10000000);
	memcpy(payload, load_address, payload_len);
	return 0;
}

int menu_commands_init() {
	add_command("load-payload", &load_payload_command, NULL);
	add_command("load", &load_command, NULL);
	return 0;
}