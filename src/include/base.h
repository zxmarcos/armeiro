#ifndef __ARM_BASE_H
#define __ARM_BASE_H

#define NULL	((void *) 0)

/* compiler flags */
#define __fastinline	__attribute__((always_inline))
#define BIT_MASK(l, f)	(((u32)-1 >> ((31 ^ l) & 0x1f)) & ((u32)-1 << (f & 0x1f)))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned size_t;
typedef u32 ioaddr32_t;
typedef u16 ioaddr16_t;
typedef u8 ioaddr8_t;

/* start.s */
extern void irq_enable();
extern void irq_disable();
extern void fiq_enable();
extern void fiq_disable();
/* irq.c */
void irq_enable_line(int num);
void irq_disable_line(int num);
void irq_init();
int irq_install_isr(int num, void (*isr)());
int irq_uninstall_isr(int num);

#define ERR_OK				0x00000000	/* success */
#define ERR_INVPARAM		0x00000001	/* invalid parameter */
#define ERR_NOTAVAIL		0x00000002	/* not avaliable */
#define ERR_INVDOMAIN		0x00001000	/* invalid domain */
#define ERR_INVPGDIR		0x00001001	/* invalid domain */

void memclr(void *ptr, u32 size);
void memset(void *ptr, u32 val, u32 size);
void memcpy(void *dst, void *src, u32 size);

struct cpu_ctx {
	u32 cpsr;
	u32 sp;
	u32 lr;
	u32 pc;
	u32 r[13]; /* GPR */
} __attribute__((packed));

#endif