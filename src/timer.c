/* arm timer
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <kernel/scheduler.h>
#include <kernel/irq.h>

#define TIMER_IRQ_NUM	5
#define IOBASE	0x101e3000
#define rwrite(r, v)	writel(IOBASE + r, v)
#define rread(r)		readl(IOBASE + r)

enum TimerRegisters {
	T1_LOAD		 = 0x00,
	T1_VALUE	 = 0x04,
	T1_CTRL		 = 0x08,
	T1_INTCTRL	 = 0x0c,
	T1_RIS		 = 0x10,
	T1_MIS		 = 0x14,
	T1_BGLOAD	 = 0x18,
	T2_LOAD		 = 0x20,
	T2_VALUE	 = 0x24,
	T2_CTRL		 = 0x28,
	T2_INTCTRL	 = 0x2c,
	T2_RIS		 = 0x30,
	T2_MIS		 = 0x34,
	T2_BGLOAD	 = 0x38,
};

void timer_irq_handler(struct cpu_ctx *ctx)
{
	(void) ctx;
	/* clear timer irq pending */
	rwrite(T1_INTCTRL, 0);
	scheduler();
}

void timer_init()
{
#if 0
	/* We need to disable timer before writing new values */
	rwrite(T1_CTRL, 0);
	/* Setup timer1 in periodic mode */
	rwrite(T1_BGLOAD, 0);
	rwrite(T1_LOAD, 150000/2);
	/* enable timer, configure as Periodic, div16, interrupt enable */
	rwrite(T1_CTRL, 0xE6);

	irq_install_isr(TIMER_IRQ_NUM, timer_irq_handler);
	/* enable interrupt line */
	irq_enable_line(TIMER_IRQ_NUM);
#elif 0
	sp804_reset();
	__delay_calibrate();
#endif
}

void sp804_reset()
{
	/* disable timer */
	rwrite(T1_CTRL, 0);
	rwrite(T1_LOAD, 0xffffffff);
	rwrite(T1_CTRL, 0xa2);
}

#undef IOBASE
#undef rread
#undef rwrite