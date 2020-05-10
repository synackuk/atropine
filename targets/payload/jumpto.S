.global jumpto

.code 32

jumpto:
	bl disable_unaligned_access
	bl clear_icache
	bl disable_irq
	push {r4, r5, r6, r7, lr}
	mov lr, r0
	mov r0, r1
	mov r1, #0
	mov r2, #0
	mov r3, #0
	blx lr
loop:
	b loop