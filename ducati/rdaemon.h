/* rdaemon header file */

#ifndef __RDAEMON_H__
#define __RDAEMON_H__

#define RDAEMON_PORT 43

/* message type */
enum rdaemon_msg_type {
	RDAEMON_PING,
};

/* message frame */
struct rdaemon_msg_frame {
	unsigned int msg_type;
	unsigned int data;
};

void RdaemonTask(void *pvParameters);

#endif
