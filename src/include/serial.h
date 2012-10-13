#ifndef __SERIAL_H
#define __SERIAL_H

#include "base.h"
#include "klist.h"

void serial_send_char(u32 chr);
void serial_init();

#endif