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
#include <stdint.h>
#include <includes/functions.h>
#include <includes/iboot_const.h>

void* bl_search_down(const void* start_addr, int len) {
    return pattern_search(start_addr, len, 0xD000F000, 0xD000F800, 1);
}

void* bl_search_up(const void* start_addr, int len) {
    return pattern_search(start_addr, len, 0xD000F000, 0xD000F800, -1);
}

void* find_last_LDR_rd(uintptr_t start, size_t len, const uint8_t rd) {
	for(uintptr_t i = start; i > 0; i -= sizeof(uint16_t)) {
		void* prev_ldr = pattern_search((void*) i, len, 0x00004800, 0x0000F800, -2);
		struct arm32_thumb_LDR* ldr = (struct arm32_thumb_LDR*) prev_ldr;

		if(ldr == NULL) {
			break;
		} else if(ldr->rd == rd) {
			return (void*) prev_ldr;
		}
		i = ((uintptr_t) prev_ldr - sizeof(uint16_t));
	}
	return NULL;
}

void* find_next_bl_insn_to(struct iboot_img* iboot_in, uint32_t addr) {
	for(size_t i = 0; i < iboot_in->len - sizeof(uint32_t); i++) {
		if((*(uint32_t*)((uintptr_t)iboot_in->buf + i) & 0xD000F800) == 0xD000F000) {
			void* possible_bl = resolve_bl32((void*)((uintptr_t)iboot_in->buf + i));
			uint32_t resolved = (uintptr_t) GET_IBOOT_FILE_OFFSET(iboot_in, possible_bl);
			if(resolved == addr) {
				return (void*) ((uintptr_t)iboot_in->buf + i);
			}
		}
	}
	return NULL;
}

void* find_next_CMP_insn_with_value(void* start, size_t len, const uint8_t val) {
	for(size_t i = 0; i < len; i += sizeof(uint16_t)) {
		struct arm32_thumb* insn = (struct arm32_thumb*) ((uintptr_t)start + i);
		if(insn->op == ARM32_THUMB_CMP && insn->offset == val) {
			return (void*) insn;
		}
	}
	return NULL;
}

void* find_next_LDR_insn_with_value(struct iboot_img* iboot_in, uint32_t value) {
	void* ldr_xref = (void*) memmem(iboot_in->buf, iboot_in->len, &value, sizeof(value));
	if(!ldr_xref) {
		return 0;
	}
	void* ldr_insn = ldr_to(ldr_xref);
	if(!ldr_insn) {
		return 0;
	}
	return ldr_insn;
}

void* find_next_MOVW_insn_with_value(void* start, size_t len, const uint16_t val) {
	for(size_t i = 0; i < len; i += sizeof(uint16_t)) {
		struct arm32_thumb_MOVW* candidate = (struct arm32_thumb_MOVW*) ((uintptr_t)start + i);
		if(is_MOVW_insn((void*)((uintptr_t)start + i)) && get_MOVW_val(candidate) == val) {
			return (void*) candidate;
		}
	}
	return NULL;
}

uint32_t get_iboot_base_address(void* iboot_buf) {
	if(iboot_buf) {
		return *(uint32_t*)((uintptr_t)iboot_buf + 0x20) & ~0xFFFFF;
	}
	return 0;
}

int get_os_version(struct iboot_img* iboot_in) {
	for(uint32_t i = 0; i < sizeof(iboot_intervals)/sizeof(struct iboot_interval); i++) {
		if(iboot_in->VERS >= iboot_intervals[i].low && iboot_in->VERS <= iboot_intervals[i].high) {
			return (int)iboot_intervals[i].os;
		}
	}
	return 0;
}

uint16_t get_MOVW_val(struct arm32_thumb_MOVW* movw) {
	return (uint16_t) (((movw->imm4 << 4) + (movw->i << 3) + movw->imm3) << 8) + movw->imm8;
}

uint16_t get_MOVT_W_val(struct arm32_thumb_MOVT_W* movt_w) {
	return get_MOVW_val((struct arm32_thumb_MOVW*) movt_w);
}

bool has_kernel_load(struct iboot_img* iboot_in) {
	void* kernelcache_prep_str = memstr(iboot_in->buf, iboot_in->len, KERNELCACHE_PREP_STRING);

	return (bool) (kernelcache_prep_str != NULL);
}

bool has_recovery_console(struct iboot_img* iboot_in) {
	void* entering_recovery_str = memstr(iboot_in->buf, iboot_in->len, ENTERING_RECOVERY_CONSOLE);

	return (bool) (entering_recovery_str != NULL);
}

bool has_ticket_check(struct iboot_img* iboot_in) {
	void* product_str = (char*)((uintptr_t)iboot_in->buf + 0x200);
	if(!strncmp(product_str, "iBEC", 4)) {
		return true;
	}

	return false;
}

void* iboot_memmem(struct iboot_img* iboot_in, void* pat) {
	uint32_t new_pat = (uintptr_t) GET_IBOOT_ADDR(iboot_in, pat);

	return (void*) memmem(iboot_in->buf, iboot_in->len, &new_pat, sizeof(uint32_t));
}

bool is_BW_insn(void* offset) {
	struct arm32_thumb_BW_T4* bw = (struct arm32_thumb_BW_T4*) offset;
	uintptr_t ptr = (uintptr_t) offset;
	if(bw->pad0 == 0x1E && bw->pad1 == 2 && bw->bit12 == 1 && (ptr & 1)) {
		return true;
	}
	return false;
}

bool is_LDRW_insn(void* offset) {
	struct arm32_thumb_LDR_T3* test = (struct arm32_thumb_LDR_T3*) offset;
	if(test->pad == 0xF8D) {
		return true;
	}
	return false;
}

bool is_MOVT_W_insn(void* offset) {
	struct arm32_thumb_MOVT_W* test = (struct arm32_thumb_MOVT_W*) offset;
	if(test->pad0 == 0x2C && test->pad1 == 0x1E && test->bit31 == 0) {
		return true;
	}
	return false;
}

bool is_MOVW_insn(void* offset) {
	struct arm32_thumb_MOVW* test = (struct arm32_thumb_MOVW*) offset;
	if(test->pad0 == 0x24 && test->pad1 == 0x1E && test->bit31 == 0) {
		return true;
	}
	return false;
}

bool is_IT_insn(void* offset) {
	struct arm32_thumb_IT_T1* it = (struct arm32_thumb_IT_T1*) offset;
	if(it->pad == 0xBF) {
		return true;
	}
	return false;
}

void* bw_search_down(const void* start_addr, int len) {
	return pattern_search(start_addr, len, 0x9000F000, 0xD000F800, 2);
}

void* ldr_pcrel_search_up(const void* start_addr, int len) {
    char *caddr = (char*)start_addr;
    for (uint8_t i = 0; i< 0xff; i++) {
        len -=4;
        if (len < 0)
            return NULL; //out of mem
        caddr -=4;
        
        uint32_t x = *(uint32_t*)caddr;
        if ((x & 0xF8FF0000) == (uint32_t)(0x48000000 | (i<<16)))
            return (void*)(caddr+2);
        else if ((x & 0x0000F8FF) == (0x00004800 | i))
            return (void*)(caddr);
    }
    return NULL;
}

void* ldr_search_down(const void* start_addr, int len) {
    return pattern_search(start_addr, len, 0x00004800, 0x0000F800, 1);
}

void* ldr_search_up(const void* start_addr, int len) {
    return pattern_search(start_addr, len, 0x00004800, 0x0000F800, -1);
}

void* ldr32_search_up(const void* start_addr, int len) {
	return pattern_search(start_addr, len, 0x0000F8DF, 0x0000FFFF, -1);
}

void* ldr_to(const void* loc) {
	uintptr_t xref_target = (uintptr_t)loc;
	uintptr_t i = xref_target;
	uintptr_t min_addr = xref_target - 0x420;
	for(; i > min_addr; i -= 2) {
		i = (uintptr_t)ldr_search_up((void*)i, i - min_addr);
		if (i == 0) {
			break;
		}

		uint32_t dw = *(uint32_t*)i;
		uintptr_t ldr_target = ((i + 4) & ~3) + ((dw & 0xff) << 2);
		if (ldr_target == xref_target) {
			return (void*)i;
		}
	}

	min_addr = xref_target - 0x1000;
	for(i = xref_target; i > min_addr; i -= 4) {
		i = (uintptr_t)ldr32_search_up((void*)i, i - min_addr);
		if (i == 0) {
			break;
		}
		uint32_t dw = *(uint32_t*)i;
		uintptr_t ldr_target = ((i + 4) & ~3) + ((dw >> 16) & 0xfff);
		if (ldr_target == xref_target) {
			return (void*)i;
		}
	}
	return NULL;
}

void* _memmem(const void* mem, int size, const void* pat, int size2) {
	char* cmem = (char*)mem;
	const char* cpat = (const char*)pat;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size2; ++j) {
			if (cmem[i + j] != cpat[j])
				goto next;
		}
		return (void*)(cmem + i);
	next:
		continue;
	}
	return NULL;
}

void* memstr(const void* mem, size_t size, const char* str) {
	return (void*) memmem(mem, size, str, strlen(str));
}

void* pattern_search(const void* addr, int len, uint32_t pattern, uint32_t mask, int step) {
	char* caddr = (char*)addr;
	if (len <= 0)
		return NULL;
	if (step < 0) {
		len = -len;
		len &= ~-(step+1);
	} else {
		len &= ~(step-1);
	}
	for (int i = 0; i != len; i += step) {
		uint32_t x = *(uint32_t*)(caddr + i);
		if ((x & mask) == pattern)
			return (void*)(caddr + i);
	}
	return NULL;
}

void* push_r4_r7_lr_search_up(const void* start_addr, int len) {
	// F0 B5
	return pattern_search(start_addr, len, 0x0000B5F0, 0x0000FFFF, -2);
}

void* pop_search(const void* start_addr, int len, int searchup) {
    return pattern_search(start_addr, len, 0x5E<<9, 0xFE<<8, 2 - 4 * (searchup!=0));
}

void* push_search(const void* start_addr, int len, int searchup) {
    return pattern_search(start_addr, len, 0x5A<<9, 0xFE<<8, 2 - 4 * (searchup!=0));
}

void* branch_thumb_unconditional_search(const void* start_addr, int len, int searchup) {
    return pattern_search(start_addr, len, 0x1C<<11, 0x1F<<11, 2 - 4 * (searchup!=0));
}

void* branch_thumb_conditional_search(const void* start_addr, int len, int searchup) {
    return pattern_search(start_addr, len, 0xD<<12, 0xF<<12, 2 - 4 * (searchup!=0));
}

void* branch_search(const void* start_addr, int len, int searchup) {
    void *ret = 0;
    void *tmp = 0;
    if ((tmp = branch_thumb_unconditional_search(start_addr, len, searchup))){
        if (!ret || ((!searchup && tmp < ret) || (searchup && tmp > ret)))
            ret = tmp;
    }else if ((tmp = branch_thumb_conditional_search(start_addr, len, searchup))){
        if (!ret || ((!searchup && tmp < ret) || (searchup && tmp > ret)))
            ret = tmp;
    }
    return ret;
}


/* Taken from saurik's substrate framework. (See Hooker.cpp) */
void* resolve_bl32(const void* bl) {
	union {
		uint16_t value;

		struct {
			uint16_t immediate : 10;
			uint16_t s : 1;
			uint16_t : 5;
		};
	} bits = {*(uint16_t*)bl};

	union {
		uint16_t value;

		struct {
			uint16_t immediate : 11;
			uint16_t j2 : 1;
			uint16_t x : 1;
			uint16_t j1 : 1;
			uint16_t : 2;
		};
	} exts = {((uint16_t*)bl)[1]};

	int32_t jump = 0;
	jump |= bits.s << 24;
	jump |= (~(bits.s ^ exts.j1) & 0x1) << 23;
	jump |= (~(bits.s ^ exts.j2) & 0x1) << 22;
	jump |= bits.immediate << 12;
	jump |= exts.immediate << 1;
	jump |= exts.x;
	jump <<= 7;
	jump >>= 7;

	return (void*) ((intptr_t)bl + 4 + jump);
}

void set_MOVT_W_insn_val(void* offset, uint8_t rd, uint16_t val) {
	struct arm32_thumb_MOVT_W* movt_w = (struct arm32_thumb_MOVT_W*) offset;
	memset(movt_w, 0, sizeof(struct arm32_thumb_MOVT_W));

	movt_w->pad0 = 0x2C;
	movt_w->pad1 = 0x1E;
	movt_w->rd = rd;
	movt_w->imm4 = (val >> 12);
	val &= ~(movt_w->imm4 << 12);
	movt_w->imm3 = (val >> 8);
	val &= ~(movt_w->imm3 << 8);
	movt_w->imm8 = val;

	if(get_MOVT_W_val(movt_w) != val) {
		movt_w->i = 1;
	}
}

void set_MOVW_insn_val(void* offset, uint8_t rd, uint16_t val) {
	struct arm32_thumb_MOVW* movw = (struct arm32_thumb_MOVW*) offset;
	memset(movw, 0, sizeof(struct arm32_thumb_MOVW));

	movw->pad0 = 0x24;
	movw->pad1 = 0x1E;
	movw->rd = rd;
	movw->imm4 = (val >> 12);
	val &= ~(movw->imm4 << 12);
	movw->imm3 = (val >> 8);
	val &= ~(movw->imm3 << 8);
	movw->imm8 = val;

	if(get_MOVW_val(movw) != val) {
		movw->i = 1;
	}
}
