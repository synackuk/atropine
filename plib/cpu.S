.global enable_unaligned_access
.global disable_unaligned_access
.global disable_irq
.code 32

enable_unaligned_access:
	push {r4, lr}
	mrc p15, 0, r4, c1, c0, 0
	bic r4, r4, #0x00000002
	mcr p15, 0, r4, c1, c0, 0
	pop {r4, pc}

disable_unaligned_access:	
	push {r4, lr}
	mrc p15, 0, r4, c1, c0, 0
	orr r4, r4, #0x00000002
	mcr p15, 0, r4, c1, c0, 0
	pop {r4, pc}

disable_irq:
	push {r4, lr}
	mrs r4, cpsr
	orr r4, r4, #0x80
	msr cpsr_c, r4
	pop {r4, pc}