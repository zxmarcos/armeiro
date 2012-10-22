/* VersatilePB uart0 serial
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <kernel/irq.h>
#include <kernel/types.h>
#include <errno.h>

#define __iobase		0x101f1000
#define IRQ_UART_NUM	12
#define UARTDR			0x00
#define UARTFR			0x18
#define UARTCR			0x30
#define UARTIMSC		0x38
#define UARTICR			0x44

static inline void __always_inline serial_enable_rxe(u32 v)
{
	u32 curr = readl(__iobase + UARTCR);
	if (v)
		writel(__iobase + UARTCR, curr | (1 << 9));
	else
		writel(__iobase + UARTCR, curr & ~(1 << 9));
}

static inline void __always_inline serial_enable_txe(u32 v)
{
	u32 curr = readl(__iobase + UARTCR);
	if (v)
		writel(__iobase + UARTCR, curr | (1 << 8));
	else
		writel(__iobase + UARTCR, curr & ~(1 << 8));
}

static inline void __always_inline enable_txe_irq(u32 v)
{
	u32 curr = readl(__iobase + UARTIMSC);
	if (v)
		writel(__iobase + UARTIMSC, curr | (1 << 5));
	else
		writel(__iobase + UARTIMSC, curr & ~(1 << 5));
}

static inline void __always_inline enable_rxe_irq(u32 v)
{
	u32 curr = readl(__iobase + UARTIMSC);
	if (v)
		writel(__iobase + UARTIMSC, curr | (1 << 4));
	else
		writel(__iobase + UARTIMSC, curr & ~(1 << 4));
}

static inline void __always_inline clear_txe_irq()
{
	writel(__iobase + UARTICR, (1 << 5));
}

static inline void __always_inline clear_rxe_irq()
{
	writel(__iobase + UARTICR, (1 << 6));
}

static inline void __always_inline serial_wait()
{
	do {
		(void) 0;
	} while (readl(__iobase + UARTFR) & (1 << 5));
}

static int serial_irq_handler(struct cpu_ctx *ctx)
{
	(void) ctx;
	writel(__iobase + UARTDR, readl(__iobase + UARTDR) + 1);

	clear_rxe_irq();
	return -ERR_OK;
}


void serial_init()
{
	/* enable receive irq and disable transmit */
	enable_rxe_irq(1);
	enable_txe_irq(0);

	/* install interrupt service routine */
	irq_install_isr(IRQ_UART_NUM, serial_irq_handler);
	irq_enable_line(IRQ_UART_NUM);
}

void serial_send_char(u32 chr)
{
	serial_wait();
	writeb(__iobase + UARTDR, chr & 0xff);
}
