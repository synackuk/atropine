#include <stdio.h>

#include "kernelpatcher.h"
#include "includes/patchers.h"
#include "includes/functions.h"

void* find_kernel_entry(char* kernel) {
	struct arm_thread_state* thread = (void*)((uintptr_t)find_load_command((void*)kernel, LC_UNIXTHREAD) + sizeof(struct thread_command));
	return (void*)thread->pc;
}

int patch_kernel(char* kernel, uint32_t kernel_phys_base, uint32_t kernel_virt_base, char* boot_args) {
	int ret = 0;
	uint32_t version = get_version((void*)kernel);
	if(!version) {
		return -1;
	}
	if(version >= 0x00080000) {
		dprintf("Patching platform_binary... ");
		ret = patch_platform_binary(kernel, kernel_phys_base, kernel_virt_base);
		if(ret != 0) {
			dprintf("failed\n");
			return -1;
		}
		dprintf("done\n");
		dprintf("Patching mapforio... ");
		//ret = patch_mapforio(kernel, kernel_phys_base, kernel_virt_base, boot_args);
		if(ret != 0) {
			dprintf("failed\n");
			return -1;
		}
		dprintf("done\n");
	}
	dprintf("Patching tfp0... ");
	ret = patch_tfp0(kernel, kernel_phys_base, kernel_virt_base);
	if(ret != 0) {
		dprintf("failed\n");
		return -1;
	}
	dprintf("done\n");
	
	dprintf("Patching mount... ");
	ret = patch_mount(kernel, kernel_phys_base, kernel_virt_base, boot_args);
	if(ret != 0) {
		dprintf("failed\n");
		return -1;
	}
	dprintf("done\n");
	
	dprintf("Patching amfi... ");
	ret = patch_amfi(kernel, kernel_phys_base, kernel_virt_base);
	if(ret != 0) {
		dprintf("failed\n");
		return -1;
	}
	dprintf("done\n");

	dprintf("Patching i_can_has_debugger... ");
	ret = patch_i_can_has_debugger(kernel, kernel_phys_base, kernel_virt_base);
	if(ret != 0) {
		dprintf("failed\n");
		return -1;
	}
	dprintf("done\n");
	
	dprintf("Patching sandbox... ");
	ret = patch_sandbox(kernel, kernel_phys_base, kernel_virt_base);
	if(ret != 0) {
		dprintf("failed\n");
		return -1;
	}
	dprintf("done\n");
	return 0;
}