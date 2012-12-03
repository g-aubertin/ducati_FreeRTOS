/* mailbox routines */

#include "common.h"
#include "interrupt.h"
#include "trace.h"
#include "mailbox.h"

/* M3 is user2 of the mailbox subsystem
 * M3 uses mailbox 0, host uses 1 */

// Mailbox register
#define MAILBOX_BASEADDR        (0xAA0F4000)

#define MAILBOX_MESSAGE(M)      ((MAILBOX_BASEADDR + 0x040) + 0x4*M)
#define MAILBOX_FIFOSTATUS(M)   ((MAILBOX_BASEADDR + 0x080) + 0x4*M)
#define MAILBOX_STATUS(M)       ((MAILBOX_BASEADDR + 0x0C0) + 0x4*M)
#define MAILBOX_REG_VAL(M)      (0x1 << (2 * M))

#define MAILBOX_IRQSTATUS_CLR_M3    (MAILBOX_BASEADDR + 0x124)
#define MAILBOX_IRQENABLE_SET_M3    (MAILBOX_BASEADDR + 0x128)
#define MAILBOX_IRQENABLE_CLR_M3    (MAILBOX_BASEADDR + 0x12C)

void enable_mailbox_irq(void)
{
	write32(MAILBOX_IRQENABLE_SET_M3, 0x1);
}

unsigned int mailbox_read(void)
{
	volatile unsigned int msg;

	// look at the msg status
	msg = read32(MAILBOX_STATUS(HOST_TO_M3_MBX));
	trace_printf("mailbox status : ");
	trace_value(msg);

	// look at the fifo status
	msg = read32(MAILBOX_FIFOSTATUS(HOST_TO_M3_MBX));
	trace_printf("fifo status : ");
	trace_value(msg);

	// finally, get the message
	msg = read32(MAILBOX_MESSAGE(HOST_TO_M3_MBX));
	trace_printf("mailbox register dump : ");
	trace_value(msg);
	write32(MAILBOX_IRQSTATUS_CLR_M3, 0x1);

	return msg;
}

void mailbox_send(unsigned int target, unsigned int msg)
{
	write32(MAILBOX_MESSAGE(target), msg);
}

void mailbox_kick(unsigned int target)
{
	write32(MAILBOX_MESSAGE(target), 0);
}
