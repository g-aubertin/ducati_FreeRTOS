#ifndef _RPMSG_H_
#define _RPMSG_H_

#include "FreeRTOS.h"
#include "queue.h"

struct service {
	char name[32];
	unsigned int port;
	xQueueHandle *queue;
};

void rpmsg_service_register(struct service *serv);

#endif // _RPMSG_H_
