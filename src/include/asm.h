#ifndef __ASM_H
#define __ASM_H

#define ENTRY(name)	.text; .global name; name:
#define IMPORT(name) .extern name

#define	CPSR_MODE_MASK	0x1f
#define CPSR_USR_MODE	0x10
#define CPSR_FIQ_MODE	0x11
#define CPSR_IRQ_MODE	0x12
#define CPSR_SVC_MODE	0x13
#define CPSR_ABT_MODE	0x17
#define CPSR_UND_MODE	0x1b
#define CPSR_SYS_MODE	0x1f

#define CPSR_IRQ_DISABLE	0x80
#define CPSR_FIQ_DISABLE	0x40

#endif