#include <plib.h>
#include <constants.h>
#include <command.h>
#include <command.h>


static int load_payload_command(int argc, command_args* argv) {
	size_t payload_len = get_env_uint("filesize");
	uint32_t* payload = (uint32_t*)((uintptr_t)0x10000000 + 0x70000);
	memcpy(payload, load_address, payload_len);
	return 0;
}

int menu_commands_init() {
	add_command("load-payload", &load_payload_command, NULL);
	return 0;
}