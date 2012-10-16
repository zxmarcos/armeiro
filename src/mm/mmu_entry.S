#include <asm/asm.h>

ENTRY(__mmu_getttbl)
	mrc		p15, #0, r0, c2, c0, #0
	mov		pc, lr
	
ENTRY(__mmu_setttbl)
	mcr		p15, #0, r0, c2, c0, #0
	mov		pc, lr

ENTRY(__mmu_getfsr)
	mrc		p15, #0, r0, c5, c0, #0
	mov		pc, lr

ENTRY(__mmu_getfar)
	mrc		p15, #0, r0, c6, c0, #0
	mov		pc, lr

ENTRY(__mmu_getdomain)
	mrc		p15, #0, r0, c3, c0, #0
	mov		pc, lr

ENTRY(__mmu_setdomain)
	mcr		p15, #0, r0, c3, c0, #0
	mov		pc, lr

ENTRY(__mmu_getcr)
	mrc		p15, #0, r0, c1, c0, #0
	mov		pc, lr

ENTRY(__mmu_setcr)
	mcr		p15, #0, r0, c1, c0, #0
	mov		pc, lr


