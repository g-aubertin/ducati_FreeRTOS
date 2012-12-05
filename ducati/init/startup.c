/* startup.c */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "trace.h"
#include "mailbox.h"
#include "common.h"
#include "interrupt.h"
#include "virtio.h"

#define STACK_SIZE 256
static unsigned long sys_stack[STACK_SIZE];

extern unsigned long _bss;
extern unsigned long _ebss;

extern xQueueHandle MboxQueue;

/* we must wait for the vring to be initialised by the host */
unsigned int init_done = 0 ;

/* FreeRTOS Port for m3 */
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler( void );

extern int main(void);

void ResetISR(void)
{
	unsigned int *bss;

	/* set bss to zero and jump to main */

	bss = &_bss ;
	while (bss < &_ebss)
		*bss++ = 0;

	main();
}
static void ExceptionHandler(void)
{
	trace_printf("Exception Handler !!!");
	while(1);
}

static void NmiSR(void)
{
	trace_printf("NMI ISR !!!");
	while(1);
}

static void FaultISR(void)
{
	trace_printf("Fault ISR !!!");
	while(1);
}

static void IntDefaultHandler(void)
{
	trace_printf("Default Handler !!!");
	while(1);
}

static void MailBoxHandler(void)
{
	unsigned int ret, msg;
	struct virtqueue_buf virtq_buf;

	trace_printf("mailbox irq received ! \n");

	/* clean the mailbox irq */
	msg = mailbox_read();

	/* clear the NVIC mailbox irq */
	nvic_clear_irq(MAILBOX_IRQ);

	xQueueSend(MboxQueue, &msg, portMAX_DELAY);

	switch(msg) {

	case RP_MBOX_ECHO_REQUEST :
		mailbox_send(M3_TO_HOST_MBX, RP_MBOX_ECHO_REPLY );
		break;

	case 0x1:
		ret = virtqueue_get_avail_buf(&virtqueue_list[msg], &virtq_buf);
		trace_printf("buffer returned from get_avail_buf: ");
		trace_value((unsigned int)(virtq_buf.buf_ptr));
		virtqueue_add_used_buf(&virtqueue_list[msg], virtq_buf.head);
		break;

	case 0x0:
		trace_printf("mailbox msg = 0 \n");
		if (init_done == 0)
			init_done = 1 ;
		break;
	}
}

void *vector_table[] __attribute__ ((section(".vectors"))) = {
    (void (*)(void))((unsigned long)sys_stack + sizeof(sys_stack)),
                                            // The initial stack pointer
    ResetISR,                               // The reset handler 1
    NmiSR,                                  // The NMI handler 2
    FaultISR,                               // The hard fault handler 3
    ExceptionHandler,                       // The MPU fault handler 4
    ExceptionHandler,                       // The bus fault handler 5
    ExceptionHandler,                       // The usage fault handler 6
    0,                                      // Reserved 7
    0,                                      // Reserved 8
    0,                                      // Reserved 9
    0,                                      // Reserved 10
    vPortSVCHandler,                        // SVCall handler 11
    ExceptionHandler,                       // Debug monitor handler 12
    0,                                      // Reserved 13
    xPortPendSVHandler,                     // The PendSV handler 14
    xPortSysTickHandler,                    // The SysTick handler 15

/* end of the exception vectors, beginning of the interrupt vectors */
/* refers to omap4460 TRM 17.3.3 */

    IntDefaultHandler,                      // XLATE_MMU_FAULT / irq #0
    IntDefaultHandler,                      // SHARED_CACHE_MMU_CPU_INT
    IntDefaultHandler,                      // CTM_TIM_EVENT_1
    IntDefaultHandler,			    // HWSEM_M3_IRQ (Semaphore interrupt)
    IntDefaultHandler,                      // IC_NEMUINTR (ICE crusher)
    IntDefaultHandler,                      // IMP_FAULT (interconnect fault) / irq #5
    IntDefaultHandler,                      // CTM_TIM_EVENT_2
    IntDefaultHandler,			    //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      // irq #10
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      // irq #15
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      // irq #20
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      // irq #25
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      // irq #30
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    MailBoxHandler,                         // Mailbox irq #34
    IntDefaultHandler,                      // irq #35
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler,                      //
    IntDefaultHandler                       // irq #40
};
