#include <plib.h>
#include <iboot.h>
#include <patchers.h>
#include <jumpto.h>

static void* iboot_entry;

int iboot_init(boot_args* args) {
	enable_unaligned_access();
	iboot_entry = args;
	return 0;
}

void iboot_boot(int should_patch) {
	int ret = 0;
	if(should_patch) {
		ret = patch_iboot(iboot_entry);
		if(ret != 0) {
			panic("Failed to patch iBoot");
		}
	}
	disable_unaligned_access();
	jumpto(iboot_entry, NULL);
}