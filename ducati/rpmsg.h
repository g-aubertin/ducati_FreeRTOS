#ifndef _RPMSG_H_
#define _RPMSG_H_

struct service {
	char name[32];
	unsigned int port;
	xQueueHandle queue;
};

void namemap_register(struct service *serv);

#endif // _RPMSG_H_
