/* rdaemon.c */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>
#include "rpmsg.h"
#include "rdaemon.h"

extern xSemaphoreHandle InitDoneSemaphore;
xQueueHandle RdaemonQueue;

void RdaemonTask(void *pvParameters)
{
	struct service serv;
	struct rdaemon_msg_frame *payload;

	RdaemonQueue = xQueueCreate( 32, sizeof( unsigned int* ) );

	strncpy(serv.name, "rpmsg-rdaemon", 32);
	serv.port = RDAEMON_PORT;
	serv.queue = &RdaemonQueue;

	xSemaphoreTake(InitDoneSemaphore, portMAX_DELAY);

	namemap_register(&serv);

	xSemaphoreGive(InitDoneSemaphore);

	for (;;) {
		xQueueReceive(RdaemonQueue, payload, portMAX_DELAY);

		payload = (struct rdaemon_msg_frame*)data;
		trace_printf("rdaemon callback ");

	if (payload->msg_type == RDAEMON_PING) {
		trace_printf("rdaemon ping : ");
		trace_value(payload->data) ;
	}


	while(1);
}
