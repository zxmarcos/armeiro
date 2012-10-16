#ifndef __SERIAL_H
#define __SERIAL_H

#include <kernel/types.h>
#include "klist.h"

void serial_send_char(u32 chr);
void serial_init();

#endif