#ifndef __KERNEL_TYPES_H
#define __KERNEL_TYPES_H

#define NULL	((void *) 0)

/* compiler flags */
#define __packed	__attribute__((packed))
#define __always_inline	__attribute__((always_inline))
/* macro to create bitmasks */
#define BIT_MASK(l, f)	\
	(((u32) -1 >> ((31 ^ l) & 0x1f)) & ((u32) -1 << (f & 0x1f)))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned size_t;



#endif