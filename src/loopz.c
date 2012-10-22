
#include <asm/platform.h>
#include <kernel/rtc.h>
#include <kernel/irq.h>
#include <errno.h>

volatile int __loopz_wait0 = 1;
volatile int __loopz_wait1 = 1;

u32 __loopz_per_sec = 0;
u32 __loopz_per_ms = 0;
u32 __loopz_per_ns = 0;
u32 __loopz_cpufreq = 0;


u32 __loopz_wait(volatile int *w0, volatile int *w1);
u32 __loopz_atomic_cycles();

static int __loopz_event_tick()
{
	static int ncall = 0;
	switch (ncall) {
		case 0: {
			/* start counter */
			__loopz_wait0 = 0;
			ncall = 1;
			return RTC_EVENT_CONTINUE;
		}
		case 1: {
			/* stop counter */
			__loopz_wait1 = 0;
			ncall = 0;
			return RTC_EVENT_DONE;
		}
	}
	return 0;
}

int loopz_init()
{
	/* we need to enable irq */
	irq_enable();
	if (!rtc_has_device()) {
		printk("loopz: no rtc device!\n");
		return -ERR_NOTAVAIL;
	}
	struct rtc_event ev = {
		.proc = __loopz_event_tick,
		.tick_load = 1,
		.tick_counter = 1,
	};

	__loopz_wait0 = 1;
	__loopz_wait1 = 1;

	printk("loopz: calibration start...\n");
	rtc_register_event(&ev);
	__loopz_per_sec = __loopz_wait(&__loopz_wait0, &__loopz_wait1);

	__loopz_per_ms = __loopz_per_sec / 1000;
	__loopz_per_ns = __loopz_per_ms / 1000;
	__loopz_cpufreq = (__loopz_per_sec * __loopz_atomic_cycles()) / 1000000;
	printk("loopz: %d/s, %d/ms, %d/ns\n\tcpufreq: %dMHz\n", __loopz_per_sec,
		__loopz_per_ms, __loopz_per_ns, __loopz_cpufreq);
	return -ERR_OK;
}