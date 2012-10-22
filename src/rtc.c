/* Realtime clock events
 * Marcos Medeiros
 */
#include <kernel/types.h>
#include <kernel/rtc.h>
#include <asm/platform.h>
#include <errno.h>
#include <klist.h>

static klist_t __rtc_events;
static int ev_id_counter = 0;
static struct rtc_device *__rtcdev = NULL;
static u32 rtc_tick_counter = 0;

int rtc_init()
{
	klist_init(&__rtc_events);
	return -ERR_OK;
}

int rtc_register_device(struct rtc_device *dev)
{
	if (!dev)
		return -ERR_INVPARAM;
	if (!__rtcdev) {
		printk("rtc: register device %s\n", dev->name);
		__rtcdev = dev;
		__rtcdev->start();
		return -ERR_OK;
	}
	return -ERR_NOTAVAIL;
}

int rtc_has_device()
{
	return (__rtcdev) ? 1 : 0;
}

int rtc_tick_event()
{
	rtc_tick_counter++;
	struct rtc_event *event;
	klist_reset(&__rtc_events);
	u32 lsize = klist_size(&__rtc_events);

	for (u32 i = 0; i < lsize; i++) {
		/* current node in list */
		event = (struct rtc_event *) klist_entry(&__rtc_events);
		printk("rtc: ev %x, tick %d\n", event, event->tick_counter);
		if (event) {
			event->tick_counter--;
			if (event->tick_counter != 0) {
				klist_next(&__rtc_events);
				continue;
			}
			/* event timer expired */
			event->tick_counter = event->tick_load;
			if (event->proc) {
				if (!event->proc()) {
					klist_remove(&__rtc_events, event);
					continue;
				}
			}
		}
		klist_next(&__rtc_events);
	}
	return -ERR_OK;
}

int rtc_register_event(struct rtc_event *ev)
{
	if (!ev)
		return -ERR_INVPARAM;
	klist_add_tail(&__rtc_events, ev);
	ev->id = ++ev_id_counter;
	/* reload counter */
	ev->tick_counter = ev->tick_load;
	return ev->id;
}

int rtc_sectotime(u32 data, struct sys_time *ts)
{
	if (!ts)
		return -ERR_INVPARAM;
	ts->year = 1969 + data / 31104000;
	ts->month = (data % 31104000) / 60 / 60 / 30 / 12;
	ts->day = (data % 2592000) / 60 / 60 / 30;
	ts->hour = (data % 86400) / 60 / 60;
	ts->minute = (data % 3600) / 60;
	ts->second = data % 60;
	return -ERR_OK;
}