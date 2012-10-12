/* kernel printk implementation
 * Marcos Medeiros
 */
#include "io.h"
#include "platform.h"
#include "serial.h"
#include <stdarg.h>

static inline void putk(u32 c)
{
	serial_send_char(c);
}

static inline void printhexU(u32 val)
{
	static const char *hexn = "0123456789ABCDEF";
	int i = 32;
	int k = 7;
	while (i >= 4) {
		char c = hexn[(val >> (k * 4)) & 0xF];
		putk(c);
		k--;
		i -= 4;
	}
}

static inline void printhexL(u32 val)
{
	static const char *hexn = "0123456789abcdef";
	int i = 32;
	int k = 7;
	while (i >= 4) {
		char c = hexn[(val >> (k * 4)) & 0xF];
		putk(c);
		k--;
		i -= 4;
	}
}

static inline void printdec(u32 val)
{
	u32 chr[11];
	memclr(chr, 11*sizeof(u32));
	int i = 0;
	do {
		chr[i] = 0x30 + (val % 10);
		val /= 10;
		i++;
	} while ((val != 0) && (i < 11));
	i--;
	while (i >= 0) {
		putk(chr[i]);
		i--;
	}
}

static inline void printbinary(u32 val, int size)
{
	if (!size)
		return;
	size -= 1;
	if (size > 32)
		size = 32;
	int i = size;
	while (i >= 0) {
		putk(0x30 + ((val >> i) & 1));
		i--;
	}
}


void printk(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	while (*fmt) {
		if (*fmt != '%') {
			putk(*fmt);
			if (*fmt == '\n')
				putk('\r');
			fmt++;
		} else {
			fmt++;
			if (*fmt == '%') {
				putk(*fmt);
				fmt++;
			} else
			if (*fmt == 'd') {
				printdec(va_arg(va, int));
				fmt++;
			} else
			if (*fmt == 'x') {
				printhexL(va_arg(va, int));
				fmt++;
			} else
			if (*fmt == 'X') {
				printhexU(va_arg(va, int));
				fmt++;
			} else
			if (*fmt == 's') {
				printk(va_arg(va, const char *));
				fmt++;
			} else
			if (*fmt == 'b') {
				int size = 0;
				fmt++;
				if (*fmt == 'b')
					size = 8;
				else
				if (*fmt == 'w')
					size = 16;
				else
				if (*fmt == 'd')
					size = 32;
				else {
					fmt-=2;
					size = 32;
				}
				fmt++;
				printbinary(va_arg(va, int), size);
			}
		}
	}
	va_end(va);
}
