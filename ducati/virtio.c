#include "virtio_ring.h"
#include "virtio.h"

// vring device addresses
#define VRING0_DA          0xA0000000
#define VRING1_DA          0xA0004000

// rpmsg values / to be aligned with kernel definitions
#define RP_MSG_NUM_BUFS     256
#define RP_MSG_BUF_SIZE     (512)
#define PAGE_SIZE           (4096)

struct vring vring_rx, vring_tx;

// physical to device address translation
static unsigned int pa_to_da(unsigned int addr)
{
	return ((addr & 0x000fffffU) | 0xa0000000U);

}

void virtqueue_init(void)
{
        // we need 2 virtqueues : one for receiving(0) and one for sending(1)

	// transmit vq
	vring_init(&vring_tx, RP_MSG_NUM_BUFS, (void *)VRING0_DA, PAGE_SIZE);
	virtqueue_list[0].id = 0 ;
	virtqueue_list[0].vring = &vring_tx;
	virtqueue_list[0].last_available = 0;

	// receive vq
	vring_init(&vring_rx, RP_MSG_NUM_BUFS, (void *)VRING1_DA, PAGE_SIZE);
	virtqueue_list[1].id = 1 ;
	virtqueue_list[1].vring = &vring_rx;
	virtqueue_list[1].last_available = 0;
}

int virtqueue_get_avail_buf(struct virtqueue *vq, struct virtqueue_buf *virtq_buf)
{
	unsigned int head, buf_position;
	unsigned int buf_pa;

	// check if new message
	if (vq->last_available == vq->vring->avail->idx) {
		trace_printf("no new message to process \n");
		return (-1);
	}

	// we need to get the buffer token in avail vring
	buf_position = vq->last_available++ % vq->vring->num ;

	// then we use it to get the head
	head = vq->vring->avail->ring[buf_position];

	// the head gives us the buf addr in the vring_descriptor
	buf_pa = (unsigned int)vq->vring->desc[head].addr;

	trace_printf("get_avail_buf pa addr: ");
	trace_value(buf_pa);

	virtq_buf->buf_ptr = (unsigned int *)pa_to_da(buf_pa);
	virtq_buf->head = head;

	trace_printf("get_avail_buf da addr: ");
	trace_value(virtq_buf->buf_ptr);

	return 0;
}

void virtqueue_add_used_buf(struct virtqueue *vq, unsigned int head)
{
	/* we need to notify that we have processed the buffer
	 * for that, we need to get the used vring, and get the idx stored in the vring
         * (not the local copy). */

	struct vring_used_elem *used_vring;
	unsigned int buf_position ;

	// first, calculate the buffer number in used vring using the usual modulus and id
	buf_position = vq->vring->used->idx % vq->vring->num;

	// then, map the appropriate vring_used_elem
	used_vring = &vq->vring->used->ring[buf_position];

	// then, in the vring_used_elem, we set the id and len
	used_vring->id = head;
	used_vring->len = RP_MSG_BUF_SIZE;

	// increment the vring's used id to notify the linux host
	vq->vring->used->idx++;
}

void virtqueue_kick(unsigned int target)
{
	mailbox_send(target, 0);
}
