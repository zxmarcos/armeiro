/*
 * IRQ handling for versatilepb
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <asm/context.h>
#include <kernel/irq.h>
#include <errno.h>
#include <memory.h>


isr_callback_t isr_handler_table[32] = { NULL };

struct irq_ctrl *__irqctrl = NULL;


void irq_setup_ctrl(struct irq_ctrl *ctrl)
{
	printk("Initializing irq controller...\n");
	__irqctrl = ctrl;
	__irqctrl->init(ctrl);
}

/* IRQ Handler ISR dispatcher
 * called from irq_handler_entry (start.s)
 */
void irq_handler(struct cpu_ctx *cpu)
{
	if (__irqctrl) {
		if (__irqctrl->handler)
			__irqctrl->handler(cpu);
	}
}

int __irq_call_isr(int num, struct cpu_ctx *cpu)
{
	if (num > 31) {
		printk("invalid irq number!\n");
		return -ERR_INVPARAM;
	}
	if (isr_handler_table[num])
		isr_handler_table[num](cpu);
	return -ERR_OK;
}

void irq_init()
{
	memclr(isr_handler_table, sizeof(isr_callback_t) * 32);
}


int irq_install_isr(int num, isr_callback_t isr)
{
	if (num > 31)
		return -ERR_INVPARAM;

	if (isr_handler_table[num])
		return -ERR_NOTAVAIL;

	isr_handler_table[num] = isr;
	return -ERR_OK;
}

int irq_uninstall_isr(int num)
{
	if (num > 31)
		return -ERR_INVPARAM;

	if (!isr_handler_table[num])
		return -ERR_NOTAVAIL;

	isr_handler_table[num] = NULL;
	return -ERR_OK;
}

void irq_enable_line(int num)
{
	if (__irqctrl) {
		if (__irqctrl->enable_line)
			__irqctrl->enable_line(num);
	}
}

void irq_disable_line(int num)
{
	if (__irqctrl) {
		if (__irqctrl->disable_line)
			__irqctrl->disable_line(num);
	}
}
