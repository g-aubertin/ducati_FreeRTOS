/* rpmsg layer */

/* includes */
#include "common.h"
#include "mailbox.h"
#include "trace.h"
#include "virtio.h"
#include "rpmsg.h"

#include <string.h>

#define MAX_SERVICE 64
#define RPMSG_HDR_SIZE 4 /*4 times 32 bits */

/* taken from linux/rpmsg.h */
struct rpmsg_hdr {
	u32 src;
	u32 dst;
	u32 reserved;
	u16 len;
	u16 flags;
	u8 data[0];
} __packed;


struct rpmsg_ns_msg {
    char name[32]; /* name of service including 0 */
    unsigned int addr;                   /* address of the service */
    unsigned int flags;                  /* see below */
} __packed;

enum rpmsg_ns_flags {
    RPMSG_NS_CREATE = 0,
    RPMSG_NS_DESTROY = 1
};

struct service service_list[MAX_SERVICE];
unsigned int service_id = 0;

void rpmsg_free_buffer(void *payload)
{

	vPortFree(payload - RPMSG_HDR_SIZE);

}

void rpmsg_dispatch_msg(unsigned int *vq_buf)
{
	struct rpmsg_hdr *hdr;
	void *payload_addr;
	unsigned int i;

	hdr = (struct rpmsg_hdr*)vq_buf;
	payload_addr = (void*)&hdr->data;

	for (i=0;i<MAX_SERVICE;i++) {
		if (service_list[i].port == hdr->dst)
			xQueueSend(*(service_list[i].queue), &payload_addr, portMAX_DELAY);
	}
}

void rpmsg_service_register(struct service *serv)
{
	unsigned int ret;
	struct rpmsg_ns_msg *ns_msg;
	struct rpmsg_hdr *hdr;
	struct virtqueue_buf virtq_buf;
	unsigned int *dst;

	/* get a free buffer on the Tx virtqueue */
	ret = virtqueue_get_avail_buf(&virtqueue_list[0], &virtq_buf);

	hdr = (struct rpmsg_hdr *)virtq_buf.buf_ptr;
	ns_msg = (struct rpmsg_ns_msg *)&hdr->data;

	hdr->src = serv->port;
	hdr->dst = 53;
	hdr->len = sizeof(struct rpmsg_ns_msg);
	hdr->flags = 0;
	hdr->reserved = 0;

	strncpy(ns_msg->name, serv->name , 32);
	ns_msg->addr = serv->port;
	ns_msg->flags = RPMSG_NS_CREATE;

	virtqueue_add_used_buf(&virtqueue_list[0], virtq_buf.head);

	/* register the service in the service list */
	strncpy(service_list[service_id].name, serv->name, 32);
	service_list[service_id].port = serv->port;
	service_list[service_id].queue = serv->queue;
	service_id++;

	/* send mailbox msg to host */
	mailbox_send(M3_TO_HOST_MBX, 0x0);
}
