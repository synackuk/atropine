#include <plib.h>
#include <target.h>
#include <drivers.h>
#include <xnu.h>
#include <iboot.h>

static void print_banner() {
	fb_print_row('=');
	printf(TARGET_NAME ".\n");
	printf("By synackuk.\n");
	printf("Part of the n1ghtshade jailbreak.\n");
	printf(TARGET_VERSION "\n");
	fb_print_row('=');
}

void main(boot_args* args) {
	uint32_t magic = *(uint32_t*) args;
	if(magic == IBOOT_MAGIC) {
		iboot_init(args);
		iboot_boot(1);
	}
	drivers_init((uint32_t*)args->Video.v_baseAddr, args->Video.v_width, args->Video.v_height);
	print_banner();
	xnu_init(args);
	xnu_boot(1, NULL);
}