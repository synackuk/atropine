.global jumpto

.code 32

jumpto:
	push {r4, r5, r6, r7, lr}
	mov lr, r0
	mov r0, r1
	mov r1, #0
	mov r2, #0
	mov r3, #0
	blx lr
loop:
	b loop