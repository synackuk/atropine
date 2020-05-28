#include <stdlib.h>
#include <stddef.h>
#include "kernelpatcher.h"
#include "includes/patchers.h"
#include "includes/functions.h"
#include "includes/finders.h"
#include "includes/sbops.h"

int patch_proc_enforce(uintptr_t phys_base, uintptr_t virt_base) {
	uint32_t* proc_enforce = find_proc_enforce(phys_base, virt_base);
	if(!proc_enforce) {
		return -1;
	}
	*(uint32_t*)proc_enforce = 0;

	return 0;

}

int patch_mapforio(uintptr_t phys_base) {
	uintptr_t* mapforio_error = find_mapforio_error(phys_base);
	if(!mapforio_error) {
		return -1;
	}
	insn_t* error_ldr;
	while((error_ldr = ldr_to(mapforio_error))) {
		insn_t* check = error_ldr;
		for(int i = 0; i < 3; i += 1) {
			if(insn_is_32bit(check)) {
				*check = NOP;
				check = &check[1];
			}
			*check = NOP;
			check = &check[1];
		}
	}
	return 0;
}

int patch_tfp0(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	uint32_t* task_for_pid = find_task_for_pid(address, phys_base, virt_base);

	if(!task_for_pid) {
		return -1;
	}

	uintptr_t i = 0;

	// To patch tfp we find the first conditional branch in task_for_pid and patch it out.

	while(i < 0x100) {
		insn_t* insn = (insn_t*)((uintptr_t)task_for_pid + i);
		if(insn_is_beqw(insn)) {
			insn[0] = NOP;
			insn[1] = NOP;
			return 0;
		}
		if(insn_is_beq(insn)) {
			*insn = NOP;
			return 0;
		}
		if(insn_is_bne(insn)) {
			*insn = 0xE006;
			return 0;
		}
		i += insn_is_32bit(insn) ? 4 : 2;
	}
	return -1;

}

int patch_amfi(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	uintptr_t* amfi_memcmp = find_amfi_memcmp(address, phys_base, virt_base);
	if(!amfi_memcmp) {
		return -1;
	}
	uintptr_t ret_0_gadget = find_ret_0_gadget(phys_base);
	if(!ret_0_gadget) {
		return -1;
	}
	*amfi_memcmp = ret_0_gadget;
	return 0;
}

int patch_mount(char* address, uintptr_t phys_base, uintptr_t virt_base, char* boot_args) {
	uintptr_t rootvnode_offset = find_rootvnode(address, phys_base, virt_base);
	if(!rootvnode_offset) {
		return -1;
	}
	char rootvnode_str[16];
	ultoa((unsigned long) rootvnode_offset, rootvnode_str, 16);
	strncat(boot_args, " rootvnode_addr=0x", 255);
	strncat(boot_args, rootvnode_str, 255);
	return 0;
}

int patch_i_can_has_debugger(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	insn_t* pe_i_can_has_debugger_func = find_pe_i_can_has_debugger(address, phys_base, virt_base);
	if(!pe_i_can_has_debugger_func) {
		return -1;
	}
	pe_i_can_has_debugger_func[0] = MOVS_R0_1;
	pe_i_can_has_debugger_func[1] = BX_LR;
	return 0;
}

int patch_platform_binary(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	insn_t* csfg_get_platform_binary_func = find_csfg_get_platform_binary(address, phys_base, virt_base);
	if(!csfg_get_platform_binary_func) {
		return -1;
	}
	csfg_get_platform_binary_func[0] = MOVS_R0_1;
	csfg_get_platform_binary_func[1] = BX_LR;
	insn_t* csproc_get_platform_binary_func = find_csproc_get_platform_binary(address, phys_base, virt_base);
	if(!csproc_get_platform_binary_func) {
		return -1;
	}
	csproc_get_platform_binary_func[0] = MOVS_R0_1;
	csproc_get_platform_binary_func[1] = BX_LR;
	return 0;

}

int patch_sandbox(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	uintptr_t* sbops = find_sbops(phys_base, virt_base);
	if(!sbops) {
		return -1;
	}

	uint32_t version = get_version((void*)address);
	if(!version) {
		return -1;
	}
	if(version >= 0x00060000 && version < 0x00070000) {
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_file_check_mmap), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_access), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_chroot), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_deleteextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_exchangedata), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_exec), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_getattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_getextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_ioctl), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_link), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_listextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_open), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_readlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setflags), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setmode), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setowner), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_truncate), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_unlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_notify_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_vnode_check_fsgetpath), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_mount_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_proc_check_setauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_proc_check_getauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops6, mpo_proc_check_fork), 0x4);
	}
	else if(version >= 0x00070000 && version < 0x00080000) {
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_file_check_mmap), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_access), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_chroot), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_deleteextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_exchangedata), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_exec), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_getattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_getextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_ioctl), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_link), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_listextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_open), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_readlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setflags), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setmode), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setowner), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_truncate), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_unlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_notify_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_vnode_check_fsgetpath), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_mount_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_proc_check_setauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_proc_check_getauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops7, mpo_proc_check_fork), 0x4);
	}
	else if(version >= 0x00080000 && version < 0x00080100) {
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_file_check_mmap), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_rename), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_access), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_chroot), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_deleteextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_exchangedata), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_exec), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_getattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_getextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_ioctl), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_link), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_listextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_open), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_readlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setflags), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setmode), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setowner), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_truncate), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_unlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_notify_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_vnode_check_fsgetpath), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_mount_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_proc_check_setauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_proc_check_getauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops80, mpo_proc_check_fork), 0x4);
	}
	else if(version >= 0x00080100 && version < 0x00090000) {
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_file_check_mmap), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_rename), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_access), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_chroot), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_deleteextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_exchangedata), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_exec), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_getattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_getextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_ioctl), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_link), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_listextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_open), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_readlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setflags), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setmode), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setowner), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_truncate), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_unlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_notify_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_vnode_check_fsgetpath), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_mount_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_proc_check_setauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_proc_check_getauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops8, mpo_proc_check_fork), 0x4);
	}
	else if(version >= 0x00090000 && version < 0x00090200) {
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_file_check_mmap), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_rename), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_access), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_chroot), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_deleteextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_exchangedata), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_exec), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_getattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_getextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_ioctl), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_link), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_listextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_open), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_readlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setflags), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setmode), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setowner), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_truncate), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_unlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_notify_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_vnode_check_fsgetpath), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_mount_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_proc_check_setauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_proc_check_getauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops90, mpo_proc_check_fork), 0x4);
	}
	else if(version >= 0x00090200 && version < 0x000A0000) {
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_file_check_mmap), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_rename), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_access), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_chroot), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_deleteextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_exchangedata), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_exec), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_getattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_getextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_ioctl), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_link), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_listextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_open), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_readlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setattrlist), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setextattr), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setflags), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setmode), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setowner), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_setutimes), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_truncate), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_unlink), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_notify_create), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_vnode_check_fsgetpath), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_mount_check_stat), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_proc_check_setauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_proc_check_getauid), 0x4);
		bzero(sbops + offsetof(struct mac_policy_ops9, mpo_proc_check_fork), 0x4);
	}
	else {
		return -1;
	}

	return 0;
}

