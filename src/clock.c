/* arm rtc
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <kernel/scheduler.h>
#include <kernel/irq.h>

#define __CYCLES_PER_LOOP	8
#define __LOOP_INTERVAL		1000000

#define __iobase	0x101e8000

#define RTC_IRQ_NUM	10

#define RTCDR	0x00	/* data register */
#define RTCMR	0x04	/* match register */
#define RTCLR	0x08	/* load register */
#define RTCCR	0x0c	/* control register */
#define RTCIMSC	0x10	/* int mask  */
#define RTCRIS	0x14	/* raw int status */
#define RTCMIS	0x18	/* masked int status */
#define RTCICR	0x1c	/* int clear status */

#define RTC_ENABLE	0x01

extern u32 __loops_wait(volatile u32 *wait, volatile u32 *running);

static inline
void __always_inline rtc_enable_irq()
{
	writel(__iobase + RTCIMSC, 1);
}

static inline
void __always_inline rtc_disable_irq()
{
	writel(__iobase + RTCIMSC, 0);
}

static inline
void __always_inline rtc_clear_irq()
{
	writel(__iobase + RTCICR, 1);
}

/* Instructions per second calibration */
static u32 __irq_ncall = 0;
volatile u32 __wait_for_start = 1;
volatile u32 __loop_running = 0;
static u32 __cpu_freq = 0;
static u32 __loops_per_ms = 0;
static u32 __loops_per_ns = 0;
static u32 __loops_per_s = 0;

static inline 
__always_inline void rtc_next_event(u32 sec)
{
	register u32 curr = readl(__iobase + RTCDR);
	curr += sec;
	writel(__iobase + RTCMR, curr);
}

/* calibrate cpu speed */
static void rtc_calibrate_irq()
{
	switch (__irq_ncall) {
		case 0:
			__wait_for_start = 0;
			__loop_running = 1;
			printk("loops calibration start...\n");
			__irq_ncall++;
			rtc_clear_irq();
			rtc_next_event(1);
			break;
		case 1:
			__loop_running = 0;
			rtc_clear_irq();
			__irq_ncall++;
			break;
	}
}

void udelay(u32 ns)
{
	u32 __loops__ = (__loops_per_ns << 1) * ns;
	cpu_delay(__loops__);
}

void __calibrate_cpu_freq()
{
	irq_install_isr(RTC_IRQ_NUM, rtc_calibrate_irq);
	irq_enable_line(RTC_IRQ_NUM);

	irq_enable();
	rtc_enable_irq();
	rtc_next_event(0);

	__loops_per_s = __loops_wait(&__wait_for_start, &__loop_running);
	__loops_per_ms = __loops_per_s / 1000;
	__loops_per_ns = __loops_per_ms / 1000;

	__cpu_freq = (__loops_per_s * __CYCLES_PER_LOOP) / __LOOP_INTERVAL;
	printk("loops_per_s: %d, cpu freq: %dMHz\n", __loops_per_s, __cpu_freq);
	printk("loops_per_ms: %d, loops_per_ns: %d\n", __loops_per_ms, __loops_per_ns);
}

void clock_init()
{
	printk("rtc starting...\n");
	writel(__iobase + RTCCR, RTC_ENABLE);
	__calibrate_cpu_freq();

	printk("wait 1s\n");
	udelay(1000*1000);
}
