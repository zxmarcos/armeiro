#include <asm/asm.h>

/*
 * Esse função tem que definir 
 */

.global __loops_per_second
__loops_per_second: .word 0


ENTRY(__loopz_wait)
1:
	ldr		r2, [r0]	@ wait for first irq
	cmp		r2, #0
	bne		1b
	mov		r0, #0
2:
						@ wait for second irq
	mov		r3, r0		@ 1 cycle
	ldr		r2, [r1]	@ 2 cycles
	cmp		r2, #0		@ 1 cycle	
	add		r0, r3, #1	@ 1 cycle
	bne		2b			@ 3 cycles
						@-------------------
						@ 8 cycles total
	mov		pc, lr

ENTRY(__loopz_atomic_cycles)
	mov		r0, #8
	mov		pc, lr

ENTRY(cpu_delay)
	cmp		r0, #0
	moveq	pc, lr
1:
	subs	r0, #1	@ 1 cycle
	bne		1b		@ 3 cycle
	mov		pc, lr