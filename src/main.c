/*
 * Kernel main function
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <asm/serial.h>
#include <asm/cpu.h>
#include <kernel/scheduler.h>
#include <kernel/mm.h>
#include <kernel/irq.h>
#include <kernel/rtc.h>
#include <kernel/loopz.h>

void kmain()
{
	arch_init();
	
	irq_init();
	
	printk("HIPSTER SO, ");
	printk("escrito por Marcos Medeiros\n");

	cpu_init();
	/* First we setup the memory manager */
	mm_init();
	rtc_init();
	
	arch_setup();

	loopz_init();
	//scheduler_init();
	irq_enable();
	for(;;) {
		(void) 0;
	}
}