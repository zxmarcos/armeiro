/* 
 * Memory functions optimized for ARM alignment
 * These functions work spliting memory word write/read and bytes write/read
 * Marcos Medeiros
 */
#include "base.h"

void memclr(void *ptr, u32 size)
{
	register u32 n32 = size >> 2;
	u32 n8 = size & 3;
	if (n32) {
		register u32 *ptr32 = ptr;
		do {
			*ptr32++ = 0;
		} while (--n32 != 0);
		ptr = ptr32;
	}
	if (n8) {
		register u8 *ptr8 = ptr;
		do {
			*ptr8++ = 0;
		} while (--n8 != 0);
	}
}

void memset(void *ptr, u32 val, u32 size)
{
	register u32 n32 = size >> 2;
	u32 n8 = size & 3;

	register u32 nval = (val & 0xff) |
						((val & 0xff) << 8) |
						((val & 0xff) << 16) |
						((val & 0xff) << 24);

	if (n32) {
		register u32 *ptr32 = ptr;
		do {
			*ptr32++ = nval;
		} while (--n32 != 0);
		ptr = ptr32;
	}
	if (n8) {
		register u8 *ptr8 = ptr;
		do {
			*ptr8++ = nval;
		} while (--n8 != 0);
	}
}

void memcpy(void *dst, void *src, u32 size)
{
	register u32 n32 = size >> 2;
	u32 n8 = size & 3;

	if (n32) {
		register u32 *dst32 = dst;
		register u32 *src32 = src;
		do {
			*dst32++ = *src32++;
		} while (--n32 != 0);
		dst = dst32;
		src = src32;
	}
	if (n8) {
		register u8 *dst8 = dst;
		register u8 *src8 = src;
		do {
			*dst8++ = *src8++;
		} while (--n8 != 0);
	}
}


void membitset(void *ptr, size_t nbits)
{
	register u32 n32 = nbits >> 5;
	u32 nb = nbits & 0x1f;
	register u32 *ptr32 = ptr;
	if (n32) {
		do {
			*ptr32++ = 0xffffffff;
		} while (--n32 != 0);
	}
	if (nb) {
		u32 v = *ptr32;
		u32 mask = (0xffffffff >> nb);
		v &= mask;
		v |= ~mask;
		*ptr32 = v;
	}
}

void membitclr(void *ptr, size_t nbits)
{
	register u32 n32 = nbits >> 5;
	u32 nb = nbits & 0x1f;
	register u32 *ptr32 = ptr;
	if (n32) {
		do {
			*ptr32++ = 0;
		} while (--n32 != 0);
	}
	if (nb) {
		*ptr32 &= (0xffffffff >> nb);
	}
}

