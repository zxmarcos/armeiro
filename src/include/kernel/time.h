#ifndef __TIME_H
#define __TIME_H

struct sys_time {
	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;
};

struct sys_time __sys_time;

#endif