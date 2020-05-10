#include <plib.h>
#include <xnu.h>
#include <patchers.h>
#include <jumpto.h>

static void* xnu_entry;
static void* xnu_base;
static uintptr_t virt_base;
static uintptr_t phys_base;
static boot_args* xnu_args;

int xnu_init(boot_args* args) {
	xnu_args = args;
	virt_base = (uintptr_t)xnu_args->virtBase;
	dprintf("virt_base: 0x%08x\n", virt_base);
	phys_base = (uintptr_t)xnu_args->physBase;
	dprintf("phys_base: 0x%08x\n", phys_base);
	xnu_base = (void*)(xnu_args->physBase + 0x1000);
	dprintf("magic: 0x%08x\n", *(uint32_t*)xnu_base);
	dprintf("xnu_base: 0x%p\n", xnu_base);
	xnu_entry = find_kernel_entry(xnu_base);
	dprintf("xnu_entry: 0x%p\n", xnu_entry);
	return 0;
}

void xnu_boot(int should_patch, char* bootargs) {
	int ret;
	dprintf("current bootargs: %s\n", xnu_args->CommandLine);
	if(bootargs) {
		strcpy(xnu_args->CommandLine, bootargs);
		dprintf("new bootargs: %s\n", xnu_args->CommandLine);
	}
	if(should_patch) {
		ret = patch_kernel(xnu_base, phys_base, virt_base, xnu_args->CommandLine);
		if(ret != 0) {
			panic("Failed to patch kernel\n");
		} 
	}
	jumpto(xnu_entry, xnu_args);
}