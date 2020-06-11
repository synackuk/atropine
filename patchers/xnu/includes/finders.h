#ifndef FINDERS_H
#define FINDERS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "includes/instructions.h"
#include "includes/mach.h"

#ifndef memmem

void *memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen);

#endif

uintptr_t* find_mapforio_error(uintptr_t phys_base);
uint32_t* find_task_for_pid(char* address, uintptr_t phys_base, uintptr_t virt_base);
uintptr_t* find_amfi_memcmp(char* address, uintptr_t phys_base, uintptr_t virt_base);
uintptr_t find_ret_0_gadget(uintptr_t phys_base);
uintptr_t* find_sbops(uintptr_t phys_base, uintptr_t virt_base);
uintptr_t find_rootvnode(char* address, uintptr_t phys_base, uintptr_t virt_base);
insn_t* find_pe_i_can_has_debugger(char* address, uintptr_t phys_base, uintptr_t virt_base);
insn_t* find_csfg_get_platform_binary(char* address, uintptr_t phys_base, uintptr_t virt_base);
insn_t* find_csproc_get_platform_binary(char* address, uintptr_t phys_base, uintptr_t virt_base);
insn_t* find_cs_entitlement_flags(char* address, uintptr_t phys_base, uintptr_t virt_base);

#endif