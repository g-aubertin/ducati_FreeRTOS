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

void RdaemonTask(void *pvParameters)
{
	char *ns_name = "rpmsg-rdaemon";
	unsigned int len ; 

	xSemaphoreTake(InitDoneSemaphore, portMAX_DELAY);

	len = strlen(ns_name);
	namemap_register(ns_name, RDAEMON_PORT);

	xSemaphoreGive(InitDoneSemaphore);

	while(1);
}
