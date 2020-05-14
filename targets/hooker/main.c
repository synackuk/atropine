#include <plib.h>
#include <constants.h>
#include <target.h>
#include <drivers.h>
#include <relocate.h>
#include <patchers.h>
#include <menu_commands.h>
#include <command.h>

int init = 0;

static void print_banner() {
	fb_print_row('=');
	printf(TARGET_NAME ".\n");
	printf("By synackuk.\n");
	printf("Part of the n1ghtshade jailbreak.\n");
	printf(TARGET_VERSION "\n");
	fb_print_row('=');
}

static int hooker_init() {
	int ret = 0;
	ret = constants_init();
	if(ret != 0) {
		return -1;
	}
	ret = drivers_init((uint32_t*)framebuffer_address, display_width, display_height);
	if(ret != 0) {
		return -1;
	}
	print_banner();

	ret = relocate_init();
	if(ret != 0) {
		return -1;
	}
	ret = patch_iboot((char*)base_address, 1);
	if(ret != 0) {
		return -1;
	}
	clear_icache();
	ret = menu_commands_init();
	if(ret != 0) {
		return -1;
	}
	return 0;
}

int main(int argc, command_args* argv) {
	int ret;
	if(!init) {
		ret = hooker_init();
		if(ret != 0) {
			return -1;
		}
		init = 1;
		return 0;
	}
	if(argc < 2) {
		return 0;
	}
	parse_command(argc - 1, &argv[1]);
	return 0;
}