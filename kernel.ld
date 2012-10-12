
ENTRY(boot_start)
SECTIONS {
	xKmap_start = .;
	.init 0x10000 : {
		xKmap_addr = .;
		*(.init)
	}
	.text : {
		*(.text)
	}
	.data : {
		*(.data)
	}
	.rodata : {
		*(.rodata)
	}
	.bss : {
		*(.bss)
	}
	/* processor stacks */
	. = . + 0x1000;
	Kstack_svc_addr = .;
	. = . + 0x1000;
	Kstack_irq_addr = .;
	. = . + 0x400;
	Kstack_fiq_addr = .;
	. = . + 0x1000;
	Kstack_usys_addr = .;
	. = . + 0x1000;
	Kstack_abt_addr = .;

	.others : {
		*(*)
	}
	xKmap_end = .;
}
