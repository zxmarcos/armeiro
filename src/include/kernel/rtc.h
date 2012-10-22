#ifndef __RTC_H
#define __RTC_H

#include <kernel/types.h>
#include <kernel/time.h>

#define RTC_EVENT_DONE		0
#define RTC_EVENT_CONTINUE	1

struct rtc_device {
	const char *name;
	int (*gettime)(struct sys_time *ts);
	int (*start)();
	int (*stop)();
};

struct rtc_event {
	int (*proc)();
	int tick_counter;
	int tick_load;
	int id;
};

int rtc_init();
int rtc_has_device();
int rtc_register_device(struct rtc_device *dev);
int rtc_register_event(struct rtc_event *ev);
int rtc_remove_event(int id);
int rtc_tick_event();
int rtc_sectotime(u32 data, struct sys_time *ts);

#endif