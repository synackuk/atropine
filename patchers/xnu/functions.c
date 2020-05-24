#include "kernelpatcher.h"
#include "includes/functions.h"

static uint32_t bit_range(uint32_t x, int start, int end)
{
	x = (x << (31 - start)) >> (31 - start);
	x = (x >> end);
	return x;
}

static uint32_t ror(uint32_t x, int places)
{
	return (x >> places) | (x << (32 - places));
}

static int thumb_expand_imm_c(uint16_t imm12)
{
	if(bit_range(imm12, 11, 10) == 0)
	{
		switch(bit_range(imm12, 9, 8))
		{
			case 0:
				return bit_range(imm12, 7, 0);
			case 1:
				return (bit_range(imm12, 7, 0) << 16) | bit_range(imm12, 7, 0);
			case 2:
				return (bit_range(imm12, 7, 0) << 24) | (bit_range(imm12, 7, 0) << 8);
			case 3:
				return (bit_range(imm12, 7, 0) << 24) | (bit_range(imm12, 7, 0) << 16) | (bit_range(imm12, 7, 0) << 8) | bit_range(imm12, 7, 0);
			default:
				return 0;
		}
	} else
	{
		uint32_t unrotated_value = 0x80 | bit_range(imm12, 6, 0);
		return ror(unrotated_value, bit_range(imm12, 11, 7));
	}
}

int insn_is_32bit(insn_t* i)
{
	return (*i & 0xe000) == 0xe000 && (*i & 0x1800) != 0x0;
}

int insn_is_bne(insn_t* i) {
	return (*i & 0xFF00) == 0xD100;
}

int insn_is_beq(insn_t* i) {
	return (*i & 0xFF00) == 0xD000;
}

int insn_is_beqw(insn_t* i) {
	return ((i[0] & 0xFBC0) == 0xf000) && ((i[1] & 0xD000) == 0x8000);
}

int insn_is_ldr_literal(insn_t* i)
{
	return (*i & 0xF800) == 0x4800 || (*i & 0xFF7F) == 0xF85F;
}

int insn_ldr_literal_rt(insn_t* i)
{
	if((*i & 0xF800) == 0x4800)
		return (*i >> 8) & 7;
	else if((*i & 0xFF7F) == 0xF85F)
		return (*(i + 1) >> 12) & 0xF;
	else
		return 0;
}

int insn_ldr_literal_imm(insn_t* i)
{
	if((*i & 0xF800) == 0x4800)
		return (*i & 0xF) << 2;
	else if((*i & 0xFF7F) == 0xF85F)
		return (*(i + 1) & 0xFFF) * (((*i & 0x0800) == 0x0800) ? 1 : -1);
	else
		return 0;
}

int insn_is_add_reg(insn_t* i)
{
	if((*i & 0xFE00) == 0x1800)
		return 1;
	else if((*i & 0xFF00) == 0x4400)
		return 1;
	else if((*i & 0xFFE0) == 0xEB00)
		return 1;
	else
		return 0;
}

int insn_add_reg_rd(insn_t* i)
{
	if((*i & 0xFE00) == 0x1800)
		return (*i & 7);
	else if((*i & 0xFF00) == 0x4400)
		return (*i & 7) | ((*i & 0x80) >> 4) ;
	else if((*i & 0xFFE0) == 0xEB00)
		return (*(i + 1) >> 8) & 0xF;
	else
		return 0;
}

int insn_add_reg_rn(insn_t* i)
{
	if((*i & 0xFE00) == 0x1800)
		return ((*i >> 3) & 7);
	else if((*i & 0xFF00) == 0x4400)
		return (*i & 7) | ((*i & 0x80) >> 4) ;
	else if((*i & 0xFFE0) == 0xEB00)
		return (*i & 0xF);
	else
		return 0;
}

int insn_add_reg_rm(insn_t* i)
{
	if((*i & 0xFE00) == 0x1800)
		return (*i >> 6) & 7;
	else if((*i & 0xFF00) == 0x4400)
		return (*i >> 3) & 0xF;
	else if((*i & 0xFFE0) == 0xEB00)
		return *(i + 1) & 0xF;
	else
		return 0;
}

int insn_is_movt(insn_t* i)
{
	return (*i & 0xFBF0) == 0xF2C0 && (*(i + 1) & 0x8000) == 0;
}

int insn_movt_rd(insn_t* i)
{
	return (*(i + 1) >> 8) & 0xF;
}

int insn_movt_imm(insn_t* i)
{
	return ((*i & 0xF) << 12) | ((*i & 0x0400) << 1) | ((*(i + 1) & 0x7000) >> 4) | (*(i + 1) & 0xFF);
}

int insn_is_mov_imm(insn_t* i)
{
	if((*i & 0xF800) == 0x2000)
		return 1;
	else if((*i & 0xFBEF) == 0xF04F && (*(i + 1) & 0x8000) == 0)
		return 1;
	else if((*i & 0xFBF0) == 0xF240 && (*(i + 1) & 0x8000) == 0)
		return 1;
	else
		return 0;
}

int insn_mov_imm_rd(insn_t* i)
{
	if((*i & 0xF800) == 0x2000)
		return (*i >> 8) & 7;
	else if((*i & 0xFBEF) == 0xF04F && (*(i + 1) & 0x8000) == 0)
		return (*(i + 1) >> 8) & 0xF;
	else if((*i & 0xFBF0) == 0xF240 && (*(i + 1) & 0x8000) == 0)
		return (*(i + 1) >> 8) & 0xF;
	else
		return 0;
}

int insn_mov_imm_imm(insn_t* i)
{
	if((*i & 0xF800) == 0x2000)
		return *i & 0xF;
	else if((*i & 0xFBEF) == 0xF04F && (*(i + 1) & 0x8000) == 0)
		return thumb_expand_imm_c(((*i & 0x0400) << 1) | ((*(i + 1) & 0x7000) >> 4) | (*(i + 1) & 0xFF));
	else if((*i & 0xFBF0) == 0xF240 && (*(i + 1) & 0x8000) == 0)
		return ((*i & 0xF) << 12) | ((*i & 0x0400) << 1) | ((*(i + 1) & 0x7000) >> 4) | (*(i + 1) & 0xFF);
	else
		return 0;
}

struct segment_command *find_segment(struct mach_header *mh, const char *segname) {
	struct load_command *lc;
	struct segment_command *s, *fs = NULL;
	lc = (struct load_command *)((uintptr_t)mh + sizeof(struct mach_header));
	while ((uintptr_t)lc < (uintptr_t)mh + (uintptr_t)mh->sizeofcmds) {
		if (lc->cmd == LC_SEGMENT) {
			s = (struct segment_command *)lc;
			if (!strcmp(s->segname, segname)) {
				fs = s;
				break;
			}
		}
		lc = (struct load_command *)((uintptr_t)lc + (uintptr_t)lc->cmdsize);
	}
	return fs;
}

/* Find start of a load command in a macho */
struct load_command *find_load_command(struct mach_header *mh, uint32_t cmd)
{
	struct load_command *lc, *flc;
	
	lc = (struct load_command *)((uintptr_t)mh + sizeof(struct mach_header));
	
	while (1) {
		if ((uintptr_t)lc->cmd == cmd) {
			flc = (struct load_command *)(uintptr_t)lc;
			break;
		}
		lc = (struct load_command *)((uintptr_t)lc + (uintptr_t)lc->cmdsize);
	}
	return flc;
}

struct section *find_section(struct segment_command *seg, const char *name) {
	struct section *sect, *fs = NULL;
	uint32_t i = 0;
	for (i = 0, sect = (struct section *)((uintptr_t)seg + (uintptr_t)sizeof(struct segment_command));
		 i < seg->nsects;
		 i++, sect = (struct section*)((uintptr_t)sect + sizeof(struct section))) {
		if (!strcmp(sect->sectname, name)) {
			fs = sect;
			break;
		}
	}
	return fs;
}

/* Find offset of an exported symbol in a macho */
void* find_sym(struct mach_header *mh, const char *name, uintptr_t phys_base, uintptr_t virt_base) {
	struct segment_command* linkedit = find_segment(mh, SEG_LINKEDIT);
	struct symtab_command* symtab = (struct symtab_command*) find_load_command(mh, LC_SYMTAB);

	uint32_t linkedit_phys = VIRT_TO_PHYS(linkedit->vmaddr);
	
	char* sym_str_table = (char*) (((char*)(linkedit_phys - linkedit->fileoff)) + symtab->stroff);
	struct nlist* sym_table = (struct nlist*)(((char*)(linkedit_phys - linkedit->fileoff)) + symtab->symoff);
	
	for (uint32_t i = 0; i < symtab->nsyms; i++) {
		if (sym_table[i].n_value && !strcmp(name,&sym_str_table[sym_table[i].n_un.n_strx])) {
			return (void*)VIRT_TO_PHYS(sym_table[i].n_value);
		}
	}
	return 0;
}

uint32_t get_version(struct mach_header *mh) {
	struct version_min_command* vers = (struct version_min_command*) find_load_command(mh, LC_VERSION_MIN_IPHONEOS);
	return vers->version;
}
