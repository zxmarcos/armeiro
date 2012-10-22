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

	/* show some info about memory */
#if 0
	printk("Kmap_start: %x\n", Kmap_start);
	printk("Kmap_addr: %x\n", Kmap_addr);
	printk("Kmap_end: %x\n", Kmap_end);
	printk("Kmap_sp_svc: %x\n", Kmap_sp_svc);
	printk("Kmap_sp_irq: %x\n", Kmap_sp_irq);
	printk("Kmap_sp_abt: %x\n", Kmap_sp_abt);

	void *buf = page_alloc_align(0, 117);

	//*(volatile unsigned int *)(0x1000001C) = 0x2CAC; /* timing magic for SVGA 800x600 */
	*(volatile unsigned int *)(0x10120000) = 0x1313A4C4;
	*(volatile unsigned int *)(0x10120004) = 0x0505F657;
	//*(volatile unsigned int *)(0x10120008) = 0x071F1800;
	*(volatile unsigned int *)(0x10120010) = (u32) buf; /* base addr of frame buffer */
	*(volatile unsigned int *)(0x10120018) = 0x82b; /* control bits */

	memset(buf, 0xf0, 0x4000*117);
#endif
	//scheduler_init();
	irq_enable();
	for(;;) {
		(void) 0;
	}
}