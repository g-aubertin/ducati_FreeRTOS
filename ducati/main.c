
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
#include "rdaemon.h"

xQueueHandle MboxQueue;

/* we must wait for the vring to be initialised by the host */
xSemaphoreHandle InitDoneSemaphore;

static void IpcTask (void * pvParameters)
{
	unsigned int msg;
	int ret;
	struct virtqueue_buf virtq_buf;

	virtqueue_init();
	nvic_enable_irq(MAILBOX_IRQ);
	enable_mailbox_irq();

	for (;;) {
		xQueueReceive(MboxQueue, &msg, portMAX_DELAY);
		trace_printf("msg from mailbox : ");
		trace_value(msg);

		switch(msg) {

		case RP_MBOX_ECHO_REQUEST :
			mailbox_send(M3_TO_HOST_MBX, RP_MBOX_ECHO_REPLY);
			break;

		case HOST_TO_M3_VRING :
			ret = virtqueue_get_avail_buf(&virtqueue_list[msg], &virtq_buf);
			trace_printf("buffer returned from get_avail_buf: ");
			trace_value((unsigned int)(virtq_buf.buf_ptr));
			virtqueue_add_used_buf(&virtqueue_list[msg], virtq_buf.head);
			break;

		case M3_TO_HOST_VRING :
			trace_printf("kick on vq0, dropping it \n");
			xSemaphoreGive(InitDoneSemaphore);
			break;
		}

	}
	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

int main( void )
{

	trace_printf("--------\n");
	trace_printf("FreeRTOS\n");
	trace_printf("--------\n");

	MboxQueue = xQueueCreate( 32, sizeof( unsigned int* ) );
	vSemaphoreCreateBinary(InitDoneSemaphore);

	xSemaphoreTake(InitDoneSemaphore, portMAX_DELAY);

	xTaskCreate(IpcTask, "IpcTask", 100, NULL, 2, NULL);
	xTaskCreate(RdaemonTask, "RdaemonTask", 100, NULL, 2, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1);

	return 0;
}
/*-----------------------------------------------------------*/
