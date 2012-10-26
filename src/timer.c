/* arm timer
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <kernel/scheduler.h>
#include <kernel/irq.h>
#include <kernel/rtc.h>

#define TIMER_IRQ_NUM	5
#define TICKHZ			1000
#define __clk			1000000
#define __iobase		0x101e3000

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

volatile u32 __last_tick_time = 0;
volatile u32 __tick_counter = 0;

int timer_irq_handler(struct cpu_ctx *ctx)
{
	__tick_counter++;
	writel(__iobase + T1_INTCTRL, 0);
	scheduler();
	(void) ctx;
	return 0;
}

void timer_init()
{
	/* We need to disable timer before writing new values */
	writel(__iobase + T1_CTRL, 0);
	/* Setup timer1 in periodic mode */
	writel(__iobase + T1_BGLOAD, 0);
	writel(__iobase + T1_LOAD, (__clk / TICKHZ) * 10);
	/* enable timer, configure as Periodic, div1, interrupt enable */
	writel(__iobase + T1_CTRL, 0xe2);

	irq_install_isr(TIMER_IRQ_NUM, timer_irq_handler);
	/* enable interrupt line */
	irq_enable_line(TIMER_IRQ_NUM);
}
