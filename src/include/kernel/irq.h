#ifndef __IRQ_H
#define __IRQ_H

#include <kernel/types.h>

extern void irq_enable();
extern void irq_disable();
extern void fiq_enable();
extern void fiq_disable();

void irq_enable_line(int num);
void irq_disable_line(int num);
void irq_init();
int irq_install_isr(int num, void (*isr)());
int irq_uninstall_isr(int num);


#endif