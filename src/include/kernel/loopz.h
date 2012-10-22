#ifndef __LOOPZ_H
#define __LOOPZ_H

#include <kernel/types.h>

extern u32 __loopz_per_sec;
extern u32 __loopz_per_ms;
extern u32 __loopz_per_ns;
extern u32 __loopz_cpufreq;

int loopz_init();
void atomic_delay(u32 ns);

#endif