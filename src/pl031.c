/* arm pl031
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/context.h>
#include <kernel/irq.h>
#include <kernel/time.h>
#include <kernel/rtc.h>
#include <errno.h>

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


static inline void __always_inline pl031_enable_irq()
{
	writel(__iobase + RTCIMSC, 1);
}

static inline void __always_inline pl031_disable_irq()
{
	writel(__iobase + RTCIMSC, 0);
}

static inline void __always_inline pl031_clear_irq()
{
	writel(__iobase + RTCICR, 1);
}

static inline __always_inline void pl031_next_event(u32 sec)
{
	register u32 next_tick = readl(__iobase + RTCDR) + sec;
	writel(__iobase + RTCMR, next_tick);
}

static int pl031_irq(struct cpu_ctx *ctx)
{
	(void) ctx;
	rtc_tick_event();
	pl031_clear_irq();
	pl031_next_event(1);
	return -ERR_OK;
}

static int pl031_decode_time(struct sys_time *ts)
{
	if (!ts)
		return -ERR_INVPARAM;

	rtc_sectotime(readl(__iobase + RTCDR), ts);
	return -ERR_OK;
}

static int pl031_start()
{
	writel(__iobase + RTCCR, RTC_ENABLE);
	pl031_enable_irq();
	pl031_next_event(1);
	return -ERR_OK;
}

static int pl031_stop()
{
	pl031_disable_irq();
	return -ERR_OK;
}

static struct rtc_device pl031_device = {
	.gettime = pl031_decode_time,
	.start = pl031_start,
	.stop = pl031_stop,
	.name = "pl031",
};

int pl031_setup()
{
	irq_install_isr(RTC_IRQ_NUM, pl031_irq);
	irq_enable_line(RTC_IRQ_NUM);
	rtc_register_device(&pl031_device);
	return -ERR_OK;
}