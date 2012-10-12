/*
 * Kernel main function
 * Marcos Medeiros
 */
#include "io.h"
#include "platform.h"
#include "scheduler.h"
#include "memmgr.h"
#include "serial.h"
#include "cpu.h"

void kmain()
{
	irq_disable();
	irq_init();

	serial_init();

	cpu_init();

	/* First we setup the memory manager */
	mm_init();

	/* Init our scheduler timer */
	timer_init();

	/* show some info about memory */
	printk("Kmap_start: %x\n", Kmap_start);
	printk("Kmap_addr: %x\n", Kmap_addr);
	printk("Kmap_end: %x\n", Kmap_end);
	printk("Kmap_sp_svc: %x\n", Kmap_sp_svc);
	printk("Kmap_sp_irq: %x\n", Kmap_sp_irq);
	printk("Kmap_sp_abt: %x\n", Kmap_sp_abt);

	scheduler_init();
	irq_enable();

	for(;;) {
		(void) 0;
	}
}