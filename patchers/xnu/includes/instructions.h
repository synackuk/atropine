#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

typedef uint16_t insn_t;

#define NOP (0xBF00)

#define MOVS_R0_0 (0x2000)
#define MOVS_R0_1 (0x2001)
#define MOVS_R0_C (0x200C)


#define BX_LR (0x4770)

#endif