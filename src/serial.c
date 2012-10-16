/* VersatilePB uart0 serial
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <kernel/irq.h>
#include <kernel/types.h>

#define IOBASE			0x101f1000
#define rwrite(r, v)	writel(IOBASE + r, v)
#define rwrite8(r, v)	writeb(IOBASE + r, v)
#define rread(r)		readl(IOBASE + r)

#define IRQ_UART_NUM	12
#define UARTDR			0x00
#define UARTFR			0x18
#define UARTCR			0x30
#define UARTIMSC		0x38
#define UARTICR			0x44

static inline
void __always_inline serial_enable_rxe(u32 v)
{
	u32 curr = rread(UARTCR);
	if (v)
		rwrite(UARTCR, curr | (1 << 9));
	else
		rwrite(UARTCR, curr & ~(1 << 9));
}

static inline
void __always_inline serial_enable_txe(u32 v)
{
	u32 curr = rread(UARTCR);
	if (v)
		rwrite(UARTCR, curr | (1 << 8));
	else
		rwrite(UARTCR, curr & ~(1 << 8));
}

static inline
void __always_inline enable_txe_irq(u32 v)
{
	u32 curr = rread(UARTIMSC);
	if (v)
		rwrite(UARTIMSC, curr | (1 << 5));
	else
		rwrite(UARTIMSC, curr & ~(1 << 5));
}

static inline
void __always_inline enable_rxe_irq(u32 v)
{
	u32 curr = rread(UARTIMSC);
	if (v)
		rwrite(UARTIMSC, curr | (1 << 4));
	else
		rwrite(UARTIMSC, curr & ~(1 << 4));
}

static inline
void __always_inline clear_txe_irq()
{
	rwrite(UARTICR, (1 << 5));
}

static inline
void __always_inline clear_rxe_irq()
{
	rwrite(UARTICR, (1 << 6));
}

static inline
void __always_inline serial_wait()
{
	do {
		(void) 0;
	} while (rread(UARTFR) & (1 << 5));
}

void serial_irq_handler(struct cpu_ctx *ctx)
{
	(void) ctx;
	rwrite(UARTDR, rread(UARTDR) + 1);

	clear_rxe_irq();
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
	rwrite8(UARTDR, chr & 0xff);
}

#undef IOBASE
#undef rread
#undef rwrite
#undef rwrite8