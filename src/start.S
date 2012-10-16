/*
 * Kernel entry point
 * Marcos Medeiros
 */
#include "asm/asm.h"
.section .init

@ =============================================================================
@ ARM interrupt vector table
@ =============================================================================

ivector_table_start:
	ldr pc, _excpt_reset
	ldr	pc, _excpt_undefined
	ldr pc, _excpt_swi
	ldr	pc, _excpt_prefetch_abort
	ldr	pc, _excpt_data_abort
	ldr pc, _excpt_reserved
	ldr	pc, _excpt_irq
	ldr	pc, _excpt_fiq

_excpt_reset:			.word excpt_reset
_excpt_undefined:		.word excpt_undefined_caller
_excpt_swi:				.word excpt_swi_caller
_excpt_prefetch_abort:	.word excpt_prefetch_abort_caller
_excpt_data_abort:		.word excpt_data_abort_caller
_excpt_reserved:		.word excpt_reserved_caller
_excpt_irq:				.word excpt_irq_caller
_excpt_fiq:				.word excpt_fiq_caller

excpt_reset:
	b	boot_start
excpt_undefined_caller:
	b	.
excpt_prefetch_abort_caller:
	b	arm_abt
excpt_data_abort_caller:
	b	arm_abt
excpt_reserved_caller:
	b	.
excpt_irq_caller:
	b	irq_handler_entry
excpt_fiq_caller:
	b	arm_fiq
excpt_swi_caller:
	b	syscall_handler_entry

ivector_table_end:
	.word 0xdeadc0de

ENTRY(boot_start)
	ldr		sp, =Kstack_svc_addr	@ setup our svc stack
	bl		copy_ivector
	bl		setup_irq_stack
	bl		setup_fiq_stack
	bl		setup_usys_stack
	bl		setup_abt_stack
	bl		kmain
	b		.

.global Kmap_start
.global Kmap_addr
.global Kmap_end
.global Kmap_sp_svc
.global Kmap_sp_irq
.global Kmap_sp_abt
Kmap_start:	.word xKmap_start
Kmap_addr:	.word xKmap_addr
Kmap_end:	.word xKmap_end
Kmap_sp_svc:	.word Kstack_svc_addr
Kmap_sp_irq:	.word Kstack_irq_addr
Kmap_sp_abt:	.word Kstack_abt_addr

@ =============================================================================
@ ContextSwitch stuff
@ =============================================================================
.global __cpu_saved_ctx
.global __cpu_is_save
__pc: .word 0
__cpsr: .word 0
__sp: .word 0
__lr: .word 0
__r0: .word 0
__cpu_saved_ctx: .word 0
__cpu_is_save: .word 0

ENTRY(__cpu_do_context_switch)
	adr		r1, __cpsr
	ldr		r2, [r0], #4
	str		r2, [r1]
	@ set saved status register
	msr		spsr, r2
	@ load sp and save in __sp
	adr		r1, __sp
	ldr		r2, [r0], #4
	str		r2, [r1]
	@ load lr and save in __lr
	adr		r1, __lr
	ldr		r2, [r0], #4
	str		r2, [r1]
	@ load pc and save in __pc
	adr		r1, __pc
	ldr		r2, [r0], #4
	str		r2, [r1]

	mov		sp, r0
	@ load GPRs
	ldmfd	sp!, {r0-r12}
	@ save r0 for future use...
	stmfd	sp!, {r1}	@ save r1
	adr		r1, __r0	@ save r0 in __r0
	str		r0, [r1]
	ldmfd	sp!, {r1}	@ load r1

	@ reset irq stack
	mrs		r0, cpsr
	and		r0, #CPSR_MODE_MASK
	cmp		r0, #CPSR_IRQ_MODE	@ irq mode?
	ldreq	sp, =Kstack_irq_addr

	ldr		r0, __cpsr
	msr		cpsr, r0
	ldr		lr, __lr
	ldr		sp, __sp
	ldr		r0, __r0
	ldr		pc, __pc

.macro push_cpu_ctx
	stmfd	sp!, {r0-r12}
	@ save r13, we need to go to last mode
	@ r5 = spsr
	@ r1 = r13
	@ r2 = r14
	@ r0 = cpsr_current
	mrs		r0, cpsr
	mrs		r5, spsr

	and		r6, r5, #CPSR_MODE_MASK
	@ check for user mode, then switch to sys mode
	cmp		r6, #CPSR_USR_MODE
	orreq	r6, #CPSR_SYS_MODE
	
	bic		r1, r0, #CPSR_MODE_MASK @ mask mode bits
	orr		r1, #CPSR_IRQ_DISABLE @ disable irqs
	orr		r1, r6 @ set last mode

	msr		cpsr, r1				@ enter last mode
	@ save sp and lr
	mov		r1, r13					@ r1 = sp
	mov		r2, r14					@ r2 = lr

	msr		cpsr, r0				@ switch back to IRQ mode

	@ save sp,lr,pc
	stmfd	sp!, {r1-r2,lr}
	stmfd	sp!, {r5}
	mov		r0, sp
	adr		r1, __cpu_is_save
	mov		r2, #1
	str		r2, [r1]
	adr		r1, __cpu_saved_ctx
	str		r0, [r1]
.endm

.macro pop_cpu_ctx
	eor		r0, r0
	adr		r1, __cpu_is_save
	str		r1, [r0]
	add		sp, #4*4 @sp,lr,pc,cpsr
	ldmfd	sp!, {r0-r12}
.endm


ENTRY(cpu_delay)
	cmp		r0, #0
	moveq	pc, lr
1:
	subs	r0, #1	@ 1 cycle
	bne		1b		@ 3 cycle
	mov		pc, lr



@ =============================================================================
@ SYSCALL (SWI) stuff
@ =============================================================================
syscall_handler_entry:
	@ save return address
	stmfd	sp!, {lr}
	push_cpu_ctx
	ldr		r1, [lr, #-4]
	and		r1, #0xff
	bl		syscall_handler
	pop_cpu_ctx
	@ jump back to return address
	ldmfd	sp!, {pc}^	

@ =============================================================================
@ IRQ stuff
@ =============================================================================
@ Be carefull, the stack is built as presented in this sequence.
irq_handler_entry:
	sub		lr, lr, #4
	@ save return address
	stmfd	sp!, {lr}
	push_cpu_ctx
	bl		irq_handler
	pop_cpu_ctx
	@ jump back to return address
	ldmfd	sp!, {pc}^


ENTRY(irq_enable)
	mrs		r0, cpsr
	bic		r0, #CPSR_IRQ_DISABLE
	msr		cpsr, r0
	mov		pc, lr

ENTRY(irq_disable)
	mrs		r0, cpsr
	orr		r0, #CPSR_IRQ_DISABLE
	msr		cpsr, r0
	mov		pc, lr

@ Setup IRQ stack
setup_irq_stack:
	mrs		r0, cpsr
	bic		r1, r0, #CPSR_MODE_MASK
	orr		r1, #CPSR_IRQ_MODE
	msr		cpsr, r1				@ enter IRQ mode
	ldr		sp, =Kstack_irq_addr
	msr		cpsr, r0				@ switch back
	mov		pc, lr

@ =============================================================================
@ FIQ stuff
@ =============================================================================

ENTRY(fiq_enable)
	mrs		r0, cpsr
	bic		r0, #CPSR_FIQ_DISABLE
	msr		cpsr, r0
	mov		pc, lr
	
ENTRY(fiq_disable)
	mrs		r0, cpsr
	orr		r0, #CPSR_FIQ_DISABLE
	msr		cpsr, r0
	mov		pc, lr

@ Setup FIQ stack
setup_fiq_stack:
	mrs		r0, cpsr
	bic		r1, r0, #CPSR_MODE_MASK
	orr		r1, #CPSR_FIQ_MODE
	msr		cpsr, r1				@ enter FIQ mode
	ldr		sp, =Kstack_fiq_addr
	msr		cpsr, r0				@ switch back to SVC mode
	mov		pc, lr


@ Setup USER/SYSTEM stack
setup_usys_stack:
	mrs		r0, cpsr
	bic		r1, r0, #CPSR_MODE_MASK
	orr		r1, #CPSR_SYS_MODE
	msr		cpsr, r1				@ enter SYS mode
	ldr		sp, =Kstack_usys_addr
	msr		cpsr, r0
	mov		pc, lr

@ Setup ABORT stack
setup_abt_stack:
	mrs		r0, cpsr
	bic		r1, r0, #CPSR_MODE_MASK
	orr		r1, #CPSR_ABT_MODE
	msr		cpsr, r1				@ enter SYS mode
	ldr		sp, =Kstack_abt_addr
	msr		cpsr, r0
	mov		pc, lr

@ Copy our interrupt vector table to 0x00000000, because QEMU
@ load us at 0x10000
copy_ivector:
	ldr		r0, =0
	ldr		r1, =ivector_table_start
	ldr		r2, =ivector_table_end
1:
	cmp		r1, r2
	ldrlo	r3, [r1]
	strlo	r3, [r0]
	addlo	r0, #4
	addlo	r1, #4
	blo		1b
	mov		pc, lr


ENTRY(__cpu_getid)
	mrc		p15, #0, r0, c0, c0, #0
	mov		pc, lr
ENTRY(__cpu_getcachet)
	mrc		p15, #0, r0, c0, c0, #1
	mov		pc, lr
ENTRY(__cpu_gettcmst)
	mrc		p15, #0, r0, c0, c0, #2
	mov		pc, lr
