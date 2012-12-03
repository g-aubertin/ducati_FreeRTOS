#ifndef _VIRTIO_H_
#define _VIRTIO_H_

enum rpmsg_mailbox_msg {
    RP_MSG_MBOX_READY           = 0xFFFFFF00,
    RP_MSG_MBOX_STATE_CHANGE    = 0xFFFFFF01,
    RP_MSG_MBOX_CRASH           = 0xFFFFFF02,
    RP_MBOX_ECHO_REQUEST        = 0xFFFFFF03,
    RP_MBOX_ECHO_REPLY          = 0xFFFFFF04,
    RP_MBOX_ABORT_REQUEST       = 0xFFFFFF05,
    RP_MSG_FLUSH_CACHE          = 0xFFFFFF06,
    RP_MSG_BOOTINIT_DONE        = 0xFFFFFF07,
    RP_MSG_HIBERNATION          = 0xFFFFFF10,
    RP_MSG_HIBERNATION_FORCE    = 0xFFFFFF11,
    RP_MSG_HIBERNATION_ACK      = 0xFFFFFF12,
    RP_MSG_HIBERNATION_CANCEL   = 0xFFFFFF13
};

struct virtqueue {
	unsigned int id;
	struct vring *vring;
	unsigned int free_buffers;
	unsigned int last_available;
	unsigned int last_used;
};

struct virtqueue virtqueue_list[2];

struct virtqueue_buf {
	unsigned int *buf_ptr;
	unsigned int head;
};

void virtqueue_init(void);
int virtqueue_get_avail_buf(struct virtqueue *vq, struct virtqueue_buf *virtq_buf);
void virtqueue_add_used_buf(struct virtqueue *vq, unsigned int head);
void virtqueue_kick(unsigned int target);

#endif
