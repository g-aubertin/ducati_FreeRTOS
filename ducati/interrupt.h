#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define MAILBOX_IRQ 34

void nvic_enable_irq(unsigned int irq);
void nvic_clear_irq(unsigned int irq);
void nvic_force_irq(unsigned int irq);

#endif
