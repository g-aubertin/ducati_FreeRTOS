/* rpmsg layer */

/* includes */
#include "common.h"
#include "mailbox.h"
#include "trace.h"
#include "virtio.h"

#include <string.h>

// taken from linux/rpmsg.h
/**
 * struct rpmsg_hdr - common header for all rpmsg messages
 * @src: source address
 * @dst: destination address
 * @reserved: reserved for future use
 * @len: length of payload (in bytes)
 * @flags: message flags
 * @data: @len bytes of message payload data
 *
 * Every message sent(/received) on the rpmsg bus begins with this header.
 */
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

void namemap_register(char *ns_name, unsigned int port)
{
	unsigned int ret;
	struct rpmsg_ns_msg *ns_msg;
	struct rpmsg_hdr *hdr;
	struct virtqueue_buf virtq_buf;
	unsigned int *dst;

	// get the buffer
	ret = virtqueue_get_avail_buf(&virtqueue_list[0], &virtq_buf);
	trace_printf("namemap : buffer returned from get_avail_buf: ");
	trace_value((unsigned int)virtq_buf.buf_ptr);

	hdr = (struct rpmsg_hdr *)virtq_buf.buf_ptr;
	ns_msg = (struct rpmsg_ns_msg *)&hdr->data;

	trace_value((unsigned int)hdr);
	trace_value((unsigned int)ns_msg);

	hdr->src = port;
	hdr->dst = 53;
	hdr->len = sizeof(struct rpmsg_ns_msg);
	hdr->flags = 0;
	hdr->reserved = 0;

	strncpy(ns_msg->name, "rpmsg-rdaemon", 32);
	ns_msg->addr = 43;
	ns_msg->flags = RPMSG_NS_CREATE;

	virtqueue_add_used_buf(&virtqueue_list[0], virtq_buf.head);
	mailbox_send(M3_TO_HOST_MBX, 0x0);
}
