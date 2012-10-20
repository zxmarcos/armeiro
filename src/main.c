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


extern void clock_init();
void kmain()
{
	arch_init();
	irq_init();
	serial_init();
	printk("HIPSTER SO, ");
	printk("escrito por Marcos Medeiros\n");
	cpu_init();
	/* First we setup the memory manager */
	mm_init();
	clock_init();
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