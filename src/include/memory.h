#ifndef __MEMORY_H
#define __MEMORY_H

#include <kernel/types.h>

void memclr(void *ptr, u32 size);
void memset(void *ptr, u32 val, u32 size);
void memcpy(void *dst, void *src, u32 size);
void membitset(void *ptr, size_t nbits);
void membitclr(void *ptr, size_t nbits);


#endif