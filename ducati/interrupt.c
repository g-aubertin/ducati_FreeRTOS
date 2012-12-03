/* interrupt routines */

#include "common.h"
#include "interrupt.h"

// NVIC registers
#define NVIC_SET_ENABLE_REG    0xE000E100
#define NVIC_SET_PENDING_REG   0xE000E200
#define NVIC_CLEAR_PENDING_REG 0xE000E280

static void write32_nvic(unsigned int reg, unsigned int irq)
{
	unsigned int enable_bit;

	if (irq > 31){
		reg += 0x4;
		enable_bit = 0x1 << (irq - 32);
	} else {
		enable_bit = 0x1 << irq ;
	}

	write32(reg, enable_bit);
}

void nvic_enable_irq(unsigned int irq)
{
	write32_nvic(NVIC_SET_ENABLE_REG, irq);
}

void nvic_clear_irq(unsigned int irq)
{
	write32_nvic(NVIC_CLEAR_PENDING_REG, irq);
}

// force an irq to trigger
void nvic_force_irq(unsigned int irq)
{
	write32_nvic(NVIC_SET_PENDING_REG, irq);
}


