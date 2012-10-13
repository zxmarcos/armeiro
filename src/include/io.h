#ifndef __ARM_IO_H
#define __ARM_IO_H

#include "base.h"


static inline void io_write32(ioaddr32_t _addr, u32 data) {
	*((ioaddr32_t *) _addr) = data;
}
static inline void io_write16(ioaddr32_t _addr, u32 data) {
	*((ioaddr16_t *) _addr) = (data & 0x0000FFFF);
}
static inline void io_write8(ioaddr32_t _addr, u32 data) {
	*((ioaddr8_t *) _addr) = (data & 0x000000FF);
}

static inline u32 io_read32(ioaddr32_t _addr) {
	return *((ioaddr32_t *) _addr);
}
static inline u16 io_read16(ioaddr32_t _addr) {
	return *((ioaddr16_t *) _addr);
}
static inline u8 io_read8(ioaddr32_t _addr) {
	return *((ioaddr8_t *) _addr);
}


#endif