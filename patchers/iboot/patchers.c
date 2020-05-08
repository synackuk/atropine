/*
 * Copyright 2013-2016, iH8sn0w. <iH8sn0w@iH8sn0w.com>
 *
 * This file is part of iBoot32Patcher.
 *
 * iBoot32Patcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * iBoot32Patcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with iBoot32Patcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <includes/finders.h>
#include <includes/functions.h>
#include <includes/patchers.h>
#include <includes/iboot_const.h>

#define MEMMEM_RELATIVE(iboot_in, bufstart, needle, needleLen) memmem(bufstart, iboot_in->len - ((char*)(bufstart) - (char*)iboot_in->buf), needle, needleLen)


int patch_boot_args(struct iboot_img* iboot_in, const char* boot_args) {

	/* Find the pre-defined boot-args from iBoot "rd=md0 ..." */
	void* default_boot_args_str_loc = memstr(iboot_in->buf, iboot_in->len, DEFAULT_BOOTARGS_STR);
	if(!default_boot_args_str_loc) {
		return -1;
	}

	/* Find the boot-args string xref within the kernel load routine. */
	void* default_boot_args_xref = iboot_memmem(iboot_in, default_boot_args_str_loc);
	if(!default_boot_args_xref) {
		return -1;
	}

	/* If new boot-args length exceeds the pre-defined one in iBoot, we need to point the xref somewhere else... */
	if(strlen(boot_args) > strlen(DEFAULT_BOOTARGS_STR)) {

		/* Find the "Reliance on this cert..." string. */
		char* reliance_cert_str_loc = (char*) memstr(iboot_in->buf, iboot_in->len, RELIANCE_CERT_STR);
		if(!reliance_cert_str_loc) {
			return -1;
		}

		/* Point the boot-args xref to the "Reliance on this cert..." string. */
		*(uint32_t*)default_boot_args_xref = (uintptr_t) GET_IBOOT_ADDR(iboot_in, reliance_cert_str_loc);

		default_boot_args_str_loc = reliance_cert_str_loc;
	}
	strcpy(default_boot_args_str_loc, boot_args);

	/* This is where things get tricky... (Might run into issues on older loaders)*/

	/* Patch out the conditional branches... */
	void* _ldr_rd_boot_args = ldr_to(default_boot_args_xref);
	if(!_ldr_rd_boot_args) {
		uintptr_t default_boot_args_str_loc_with_base = (uintptr_t) GET_IBOOT_FILE_OFFSET(iboot_in, default_boot_args_str_loc) + get_iboot_base_address(iboot_in->buf);

		_ldr_rd_boot_args = find_next_LDR_insn_with_value(iboot_in, (uint32_t) default_boot_args_str_loc_with_base);
		if(!_ldr_rd_boot_args) {
			return -1;
		}
	}

	struct arm32_thumb_LDR* ldr_rd_boot_args = (struct arm32_thumb_LDR*) _ldr_rd_boot_args;

	/* Find next CMP Rd, #0 instruction... */
	void* _cmp_insn = find_next_CMP_insn_with_value(ldr_rd_boot_args, 0x100, 0);
	if(!_cmp_insn) {
		return -1;
	}

	void* arm32_thumb_IT_insn = _cmp_insn;


	/* Find the next IT EQ/IT NE instruction following the CMP Rd, #0 instruction... (kinda hacky) */
	while(*(uint16_t*)arm32_thumb_IT_insn != ARM32_THUMB_IT_EQ && *(uint16_t*)arm32_thumb_IT_insn != ARM32_THUMB_IT_NE) {
		arm32_thumb_IT_insn = (void*)((uintptr_t)arm32_thumb_IT_insn + 1);
	}


	/* MOV Rd, Rs instruction usually follows right after the IT instruction. */
	struct arm32_thumb_hi_reg_op* mov_insn = (struct arm32_thumb_hi_reg_op*) ((uintptr_t)arm32_thumb_IT_insn + 2);


	/* Find the last LDR Rd which holds the null string pointer... */
	int null_str_reg = (ldr_rd_boot_args->rd == mov_insn->rs) ? mov_insn->rd : mov_insn->rs;

	/* + 0x10: Some iBoots have the null string load after the CMP instruction... */
	void* ldr_null_str = find_last_LDR_rd(((uintptr_t)_cmp_insn + 0x10), 0x200, null_str_reg);
	if(!ldr_null_str) {
		return -1;
	}


	/* Calculate the new PC relative load from the default boot args xref to the LDR Rd, =null_string location. */
	uint32_t diff = (uint32_t) ((uint32_t)GET_IBOOT_FILE_OFFSET(iboot_in, default_boot_args_xref) - (uint32_t)GET_IBOOT_FILE_OFFSET(iboot_in, ldr_null_str));

	/* T1 LDR PC-based instructions use the immediate 8 bits multiplied by 4. */
	struct arm32_thumb_LDR* ldr_rd_null_str = (struct arm32_thumb_LDR*) ldr_null_str;
	ldr_rd_null_str->imm8 = (diff / 0x4);

	return 0;
}

int patch_debug_enabled(struct iboot_img* iboot_in) {

	/* Find the BL get_value_for_dtre_var insn... */
	void* get_value_for_dtre_bl = find_dtre_get_value_bl_insn(iboot_in, DEBUG_ENABLED_DTRE_VAR_STR);
	if(!get_value_for_dtre_bl) {
		return -1;
	}


	/* BL get_dtre_value --> MOVS R0, #1; MOVS R0, #1 */
	*(uint32_t*)get_value_for_dtre_bl = bswap32(0x01200120);

	return 0;
}

int patch_rsa_check(struct iboot_img* iboot_in) {
	
	/* Find the BL verify_shsh instruction... */
	void* bl_verify_shsh = find_bl_verify_shsh(iboot_in);
	if(!bl_verify_shsh) {
		return -1;
	}
	
	
	/* BL verify_shsh --> MOVS R0, #0; STR R0, [R3] */
	*(uint32_t*)bl_verify_shsh = bswap32(0x00201860);
	
	return 0;
}

int patch_ticket_check(struct iboot_img* iboot_in) {
#define pointer(p) (__pointer[0] = (uint32_t)p & 0xff, __pointer[1] = ((uint32_t)p/0x100) & 0xff, __pointer[2] = ((uint32_t)p/0x10000) & 0xff, __pointer[3] = ((uint32_t)p/0x1000000) & 0xff, _pointer)
	char __pointer[4];
	char *_pointer = __pointer;
	char *NOPstart = NULL;
	char *NOPstop = NULL;
	
	/* find iBoot_vers_str */
	const char* iboot_vers_str = memstr(iboot_in->buf, iboot_in->len, "iBoot-");
	if (!iboot_vers_str) {
		return -1;
	}
	
	
	/* find pointer to vers_str (should be a few bytes below string) */
	uint32_t vers_str_iboot = (uint32_t)GET_IBOOT_ADDR(iboot_in,iboot_vers_str);
	char *str_pointer = MEMMEM_RELATIVE(iboot_in, iboot_vers_str, pointer(vers_str_iboot), 4);
	if (!str_pointer) {
		return -1;
	}
	


	/* find 3rd xref */
	uint32_t* str_pointer_iboot = (uint32_t*)GET_IBOOT_ADDR(iboot_in,str_pointer);
	char *iboot_str_3_xref = iboot_in->buf;
	for (int i=0; i<3; i++) {
		if (!(iboot_str_3_xref = MEMMEM_RELATIVE(iboot_in, iboot_str_3_xref+1, pointer(str_pointer_iboot), 4))){
			return -1;
		}
	}
	
	
	/* find ldr rx = iboot_str_3_xref */
	char *ldr_intruction = ldr_pcrel_search_up(iboot_str_3_xref, 0x100);
	if (!ldr_intruction) {
		return -1;
	}
	
	
	char *last_good_bl = bl_search_down(ldr_intruction,0x100);
	if (!last_good_bl) {
		return -1;
	}
	last_good_bl +=4;
	
	char *next_pop = pop_search(last_good_bl,0x100,0);
	if (!next_pop) {
		return -1;
	}
	
	char *last_branch = branch_search(next_pop,0x20,1);
	char *prev_mov_r0_fail = pattern_search(next_pop, 0x20, bswap32(0x4ff0ff30), bswap32(0x4ff0ff30), -2);

	if (prev_mov_r0_fail && prev_mov_r0_fail > last_branch) {
		last_branch = prev_mov_r0_fail-2; //last branch is a BL
	}
	
	if (!last_branch) {
		return -1;
	}
	
	
	*(uint32_t*)last_good_bl = bswap32(0x4ff00000);
	last_good_bl +=4;
	
	*(uint32_t*)last_good_bl = bswap32(0x4ff00001);
	last_good_bl +=4;
	
	NOPstart = last_good_bl;
	NOPstop = last_branch+2;
	
	//because fuck clean patches
	
	while (NOPstart<NOPstop) {
		NOPstart[0] = 0x00;
		NOPstart[1] = 0xBF; //NOP
		NOPstart +=2;
	}
	
	if (*(uint32_t*)NOPstop == bswap32(0x4ff0ff30)){ //mov.w      r0, #0xffffffff
		/* mov.w      r0, #0xffffffff -->  mov.w      r0, #0x0 */
		*(uint32_t*)NOPstop = bswap32(0x4ff00000);
	}
		
	
	return 0;
}


static void* find_jumpto(struct iboot_img* iboot_in) {
	if(has_kernel_load(iboot_in)) {
		/* Find the pre-defined boot-args from iBoot "rd=md0 ..." */
		void* default_boot_args_str_loc = memstr(iboot_in->buf, iboot_in->len, "jumping into image at");
		if(!default_boot_args_str_loc) {
			return NULL;
		}
	
		/* Find the boot-args string xref within the kernel load routine. */
		void* default_boot_args_xref = iboot_memmem(iboot_in, default_boot_args_str_loc);
		if(!default_boot_args_xref) {
			return NULL;
		}
		uintptr_t* go_cmd_jump = ldr_to(default_boot_args_xref);
		if(!go_cmd_jump) {
			return NULL;
		}
		uintptr_t* go_bl_1 = bl_search_down(go_cmd_jump, 8);
		if(!go_bl_1) {
			return NULL;
		}
		uintptr_t* jumpto_bl = bl_search_down((char *)go_bl_1 + 4, 20);
		if (!jumpto_bl) {
			jumpto_bl = bw_search_down((char *)go_bl_1 + 4, 24);
			if (!jumpto_bl) {
				return NULL;
			}
		}
		uintptr_t jumpto_addr = (uintptr_t)resolve_bl32(jumpto_bl);
		if(!jumpto_addr) {
			return NULL;
		}
		jumpto_addr &= ~1;
		return (void*)jumpto_addr;
	}
	else {
		/* Find the pre-defined boot-args from iBoot "rd=md0 ..." */
		void* default_boot_args_str_loc = memstr(iboot_in->buf, iboot_in->len, "executing image...\n");
		if(!default_boot_args_str_loc) {
			return NULL;
		}
	
		/* Find the boot-args string xref within the kernel load routine. */
		void* default_boot_args_xref = iboot_memmem(iboot_in, default_boot_args_str_loc);
		if(!default_boot_args_xref) {
			return NULL;
		}
		uintptr_t* go_cmd_jump = ldr_to(default_boot_args_xref);
		if(!go_cmd_jump) {
			return NULL;
		}
		uintptr_t* go_bl_1 = bl_search_down(go_cmd_jump, 8);
		if(!go_bl_1) {
			return NULL;
		}
		uintptr_t* jumpto_bl = bl_search_down((char *)go_bl_1 + 4, 0x20);
		if (!jumpto_bl) {
			jumpto_bl = bw_search_down((char *)go_bl_1 + 4, 24);
			if (!jumpto_bl) {
				return NULL;
			}
		}
		uintptr_t jumpto_addr = (uintptr_t)resolve_bl32(jumpto_bl);
		if(!jumpto_addr) {
			return NULL;
		}
		jumpto_addr &= ~1;
		return (void*)jumpto_addr;
	}
}


typedef void (*jumpto_t)(int boot_type, void* jump_address, void* arg) __attribute__((noreturn));


static void* jumpto_func = NULL;

static uint64_t original = 0;

__attribute__((noreturn)) static void hooker(int flags, void* addr, void* arg) {
	void* payload = (void*)(0x10000000 + 0x70000);
	memcpy(jumpto_func, &original, 8);
	clear_icache();
	jumpto_t jumpto_thmb = (jumpto_t)(((uintptr_t) jumpto_func) | 1);
	if(arg) {
		jumpto_thmb(flags, payload, arg);
	}
	else {
		jumpto_thmb(flags, payload, addr);
	}
}


int patch_next_stage(struct iboot_img* iboot_in) {
	jumpto_t hook = &hooker;
	void* jumpto_addr = find_jumpto(iboot_in);
	if(!jumpto_addr) {
		return -1;
	}
	jumpto_func = GET_IBOOT_ADDR(iboot_in, jumpto_addr);
	memcpy(&original, jumpto_addr, 8);
	memcpy(jumpto_addr, "\x00\x4b\x98\x47", 4);
	memcpy((void*)((uintptr_t)jumpto_addr + 4), &hook, 4);
	return 0;
}








