
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* using gcc built-in functions */
#include <string.h>

/* resource table for rpmsg */
#include "rsc_table.h"

/* home-made */
#include "common.h"
#include "mailbox.h"
#include "trace.h"
#include "interrupt.h"
#include "virtio.h"

extern unsigned int init_done;

/*-----------------------------------------------------------*/

int main( void )
{
	trace_printf("--------\n");
	trace_printf("FreeRTOS\n");
	trace_printf("--------\n");

	virtqueue_init();

	enable_mailbox_irq();
	nvic_enable_irq(MAILBOX_IRQ);

	while(!init_done);

	namemap_register();

	while(1);

	return 0;
}
/*-----------------------------------------------------------*/
