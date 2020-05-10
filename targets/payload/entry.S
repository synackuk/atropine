#include <target.h>

.code 32

.global _start
_start:
	b reset

.code 32
reset:
 
zero_bss:
	ldr	r1, =edata
	ldr	r2, =end

zero_bss_loop:
	cmp	r1, r2
	strle	r3, [r1], #4
	blt	zero_bss_loop
	strle	r3, [r1], #4
	mcr	p15, 0, r3, c7, c5, 0
	dsb
	isb


ready:
	bl enable_unaligned_access
	b main