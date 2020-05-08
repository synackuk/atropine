#include "kernelpatcher.h"
#include "includes/finders.h"
#include "includes/functions.h"

uint32_t* find_task_for_pid(char* address, uintptr_t phys_base, uintptr_t virt_base) {

	int old = 0;

	// First we find the const section of the data segment

	struct segment_command* data_segment = find_segment((void*)address, SEGMENT_DATA);
	if(!data_segment) {
		return NULL;
	}

	struct section* const_section = find_section(data_segment, SECTION_CONST);

	if(!const_section) {
		return NULL;
	}
	void* mach_trap_0_ptr = NULL;

	uintptr_t data_phys = VIRT_TO_PHYS(data_segment->vmaddr);

	uint32_t* const_section_addr = (uint32_t*)((data_phys - data_segment->fileoff) + const_section->offset);

	// Then we search for the mach trap table

	for(uint32_t i = 0; i < const_section->size; i++) {
		mach_trap_t* potential = (void*)(const_section_addr + i);
		if(potential->num_args != 0 || (potential->handler & virt_base) != virt_base || potential->num_u32 != 0) {
			continue;
		}
		for(int j = 1; j <= 9; j += 1) {
			if(memcmp(potential, &potential[j], sizeof(mach_trap_t)) != 0) {
				goto next;
			}
		}
		mach_trap_0_ptr = potential;
		break;
		next:
		continue;
	}
	if(!mach_trap_0_ptr) {
		for(uint32_t i = 0; i < const_section->size; i++) {
			mach_trap_old_t* potential = (void*)(const_section_addr + i);
			if(potential->num_args != 0 || (potential->handler & virt_base) != virt_base ) {
				continue;
			}
			for(int j = 1; j <= 9; j += 1) {
				if(memcmp(potential, &potential[j], sizeof(mach_trap_old_t)) != 0) {
					goto next_old;
				}
			}
			mach_trap_0_ptr = potential;
			old = 1;
			break;
			next_old:
			continue;
		}
		if(!mach_trap_0_ptr) {
			return NULL;
		}
	}
	uintptr_t task_for_pid_address = 0;

	// task_for_pid is the 45th mac trap.

	if(old) {
		mach_trap_old_t* mach_trap_0 = mach_trap_0_ptr;
		mach_trap_old_t* mach_trap_45 = &mach_trap_0[45];
		task_for_pid_address = (mach_trap_45->handler) - 1;
	}
	else {
		mach_trap_t* mach_trap_0 = mach_trap_0_ptr;
		mach_trap_t* mach_trap_45 = &mach_trap_0[45];
		task_for_pid_address = (mach_trap_45->handler) - 1;
	}

	uint32_t* task_for_pid = (uint32_t*)(VIRT_TO_PHYS(task_for_pid_address));

	return task_for_pid;

}

uintptr_t* find_amfi_memcmp(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	uintptr_t memcmp_func = (uintptr_t)find_sym((void*)address, "_memcmp", phys_base, virt_base);
	if(!memcmp_func) {
		return NULL;
	}
	memcmp_func |= 1;
	uintptr_t mach_msg_rpc_from_kernel_proper_func = (uintptr_t)find_sym((void*)address, "_mach_msg_rpc_from_kernel_proper", phys_base, virt_base);
	if(!mach_msg_rpc_from_kernel_proper_func) {
		return NULL;
	}
	mach_msg_rpc_from_kernel_proper_func |= 1;

	uintptr_t search[2];
	search[0] = mach_msg_rpc_from_kernel_proper_func;
	search[1] = memcmp_func;

	uintptr_t* overwrite = memmem((void*)phys_base, KERNEL_LEN, search, 2 * sizeof(uintptr_t));

	if(!overwrite) {
		return NULL;
	}

	return &overwrite[1];
}

uintptr_t find_ret_0_gadget(uintptr_t phys_base) {
	insn_t search[2];
	search[0] = MOVS_R0_0; 
	search[1] = BX_LR;

	uintptr_t ret_0_gadget = (uintptr_t)memmem((void*)phys_base, KERNEL_LEN, search, 2 * sizeof(insn_t));

	if(!ret_0_gadget) {
		return 0;
	}

	ret_0_gadget |= 1;
	return ret_0_gadget;
}

uintptr_t* find_sbops(uintptr_t phys_base, uintptr_t virt_base) {
	
	uintptr_t* seatbelt_sandbox_str_addr = memmem((void*)phys_base, KERNEL_LEN, "Seatbelt sandbox policy", strlen("Seatbelt sandbox policy"));
	
	if(!seatbelt_sandbox_str_addr) {
		return NULL;
	}
		
	char* seatbelt_sandbox_str_xref = memmem((void*)phys_base, KERNEL_LEN, &seatbelt_sandbox_str_addr, sizeof(uintptr_t));

	if(!seatbelt_sandbox_str_xref) {
		return NULL;
	}

	uint32_t val = 1;
	
	uintptr_t* sbops_address_loc = memmem((void*)seatbelt_sandbox_str_xref, 0x10, &val, sizeof(uint32_t));

	if(!sbops_address_loc) {
		return NULL;
	}
	uintptr_t* sbops_address = (uintptr_t*)*(uintptr_t*)((uintptr_t)sbops_address_loc + 4);

	return (uintptr_t*)VIRT_TO_PHYS(sbops_address);
}

uintptr_t find_rootvnode(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	uintptr_t rootvnode_offset = (uintptr_t)find_sym((void*)address, "_rootvnode", phys_base, virt_base);
	if(!rootvnode_offset) {
		return 0;
	}
	rootvnode_offset -= (uintptr_t)address;
	return (uintptr_t)rootvnode_offset;
}

insn_t* find_pe_i_can_has_debugger(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	return (insn_t*)find_sym((void*)address, "_PE_i_can_has_debugger", phys_base, virt_base);
}

insn_t* find_csfg_get_platform_binary(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	return (insn_t*)find_sym((void*)address, "_csfg_get_platform_binary", phys_base, virt_base);
}

insn_t* find_csproc_get_platform_binary(char* address, uintptr_t phys_base, uintptr_t virt_base) {
	return (insn_t*)find_sym((void*)address, "_csproc_get_platform_binary", phys_base, virt_base);
}
