/*
 * Vectored Interrupt Controller
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <asm/context.h>
#include <kernel/irq.h>
#include <errno.h>
#include <memory.h>

#define __iobase	0x10140000


enum VICRegisters
{
	IRQ_STATUS		= 0x000,
	FIQ_STATUS		= 0x004,
	RAW_INTR		= 0x008,
	INT_SELECT		= 0x00c,
	INT_ENABLE		= 0x010,
	INT_CLEAR		= 0x014,
	SOFT_INT		= 0x018,
	SOFT_INT_CLEAR	= 0x01c,
	PROTECTION		= 0x020,
	VECT_ADDR		= 0x030,
	DEF_VECT_ADDR	= 0x034,
	VECT_ADDR_0		= 0x100,
	VECT_ADDR_1		= 0x104,
	VECT_ADDR_2		= 0x108,
	VECT_ADDR_3		= 0x10c,
	VECT_ADDR_4		= 0x110,
	VECT_ADDR_5		= 0x114,
	VECT_ADDR_6		= 0x118,
	VECT_ADDR_7		= 0x11c,
	VECT_ADDR_8		= 0x120,
	VECT_ADDR_9		= 0x124,
	VECT_ADDR_10	= 0x128,
	VECT_ADDR_11	= 0x12c,
	VECT_ADDR_12	= 0x130,
	VECT_ADDR_13	= 0x134,
	VECT_ADDR_14	= 0x138,
	VECT_ADDR_15	= 0x13c,
	VECT_CTRL_0		= 0x200,
	VECT_CTRL_1		= 0x204,
	VECT_CTRL_2		= 0x208,
	VECT_CTRL_3		= 0x20c,
	VECT_CTRL_4		= 0x210,
	VECT_CTRL_5		= 0x214,
	VECT_CTRL_6		= 0x218,
	VECT_CTRL_7		= 0x21c,
	VECT_CTRL_8		= 0x220,
	VECT_CTRL_9		= 0x224,
	VECT_CTRL_10	= 0x228,
	VECT_CTRL_11	= 0x22c,
	VECT_CTRL_12	= 0x230,
	VECT_CTRL_13	= 0x234,
	VECT_CTRL_14	= 0x238,
	VECT_CTRL_15	= 0x23c,
};

/* IRQ Handler ISR dispatcher
 * called from irq_handler_entry (start.s)
 */
static int vic_irq_handler(struct cpu_ctx *cpu)
{
 	u32 irqstatus = readl(__iobase + IRQ_STATUS);
	u32 i = 0;
	for (i = 0; i < 32; i++) {
		if (irqstatus & 1) {
			__irq_call_isr(i, cpu);
			break;
		}
		irqstatus >>= 1;
	}
	return -ERR_OK;
}

static int vic_init()
{
	return -ERR_OK;
}

static int vic_enable_line(int num)
{
	if (num > 31)
		return -ERR_INVPARAM;
	writel(__iobase + INT_ENABLE, (1 << num));
	return -ERR_OK;
}

static int vic_disable_line(int num)
{
	if (num > 31)
		return -ERR_INVPARAM;
	writel(__iobase + INT_CLEAR, (1 << num));
	return -ERR_OK;
}


static struct irq_ctrl vic_irq_ctrl = {
	.init = vic_init,
	.enable_line = vic_enable_line,
	.disable_line = vic_disable_line,
	.handler = vic_irq_handler,
	.name = "arm-vic"
};

void vic_setup()
{
	irq_setup_ctrl(&vic_irq_ctrl);
}