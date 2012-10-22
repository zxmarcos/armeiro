/* 
 * Marcos Medeiros
 */

#include <asm/io.h>
#include <asm/platform.h>
#include <asm/cpu.h>
#include <asm/context.h>
#include <asm/serial.h>
#include <kernel/irq.h>
#include <errno.h>
#include <memory.h>

void arch_init()
{
	vic_setup();
	serial_init();
}

void arch_setup()
{
	pl031_setup();
	timer_init();
}