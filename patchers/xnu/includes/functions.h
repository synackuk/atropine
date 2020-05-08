#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdlib.h>
#include <string.h>
#include "includes/instructions.h"
#include "includes/mach.h"

#define VIRT_TO_PHYS(x) (((uintptr_t)x - (uintptr_t)virt_base) + (uintptr_t)phys_base)
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

typedef int (*insn)(insn_t*);

int insn_is_32bit(insn_t* i);
int insn_is_bne(insn_t* i);
int insn_is_beq(insn_t* i);
int insn_is_beqw(insn_t* i);
int insn_is_ldr_literal(insn_t* i);
int insn_ldr_literal_rt(insn_t* i);
int insn_ldr_literal_imm(insn_t* i);
int insn_is_add_reg(insn_t* i);
int insn_add_reg_rd(insn_t* i);
int insn_add_reg_rn(insn_t* i);
int insn_add_reg_rm(insn_t* i);
int insn_is_movt(insn_t* i);
int insn_movt_rd(insn_t* i);
int insn_movt_imm(insn_t* i);
int insn_is_mov_imm(insn_t* i);
int insn_mov_imm_rd(insn_t* i);
int insn_mov_imm_imm(insn_t* i);
struct segment_command *find_segment(struct mach_header *mh, const char *segname);
struct load_command *find_load_command(struct mach_header *mh, uint32_t cmd);
struct section *find_section(struct segment_command *seg, const char *name);
void* find_sym(struct mach_header *mh, const char *name, uintptr_t phys_base, uintptr_t virt_base);
uint32_t get_version(struct mach_header *mh);
uint32_t find_kextbase(void *kernelcache);

#endif