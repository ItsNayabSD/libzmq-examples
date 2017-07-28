/* Zero MQ UDP Dish (Receiver) */


/* TODO
 * 1. Mechanism to convert hostname to IP string 
 * 2. Configuration file parsing and build program
 * 3. Build config to configure build parameters (scalability)
 * 4. Global context structure for netcomm
 * 5. Improved error handling scheme
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

// To enable DRAFT API
#define ZMQ_BUILD_DRAFT_API 1

#include <zmq.h>

#define RADIO_ADDR_LEN 256
#define BUFLEN	256
#define NUMARGS 3


void zmsgFree(void *data, void *hint) {
	free(data);
}

int main(int argc, char* argv[])
{
	zmq_msg_t zmsgBuff;

	void *ctx = zmq_ctx_new(); /* Any zmq socket requires a context */
	void *zdish = zmq_socket(ctx, ZMQ_DISH);

	if (zdish == NULL) {
		perror("Error: ");
		exit(1);
	}

	zmq_join(zdish, "Hostapd");
	//zmq_join(zdish, "Wifidog");
	/* FIXME - get the address and port as input */
	if ( zmq_bind(zdish, "udp://127.0.0.1:51717") < 0) {
		fprintf(stderr, "ERROR failed to bind zmq socket\n");			
		goto err;
	}

	int i;
	for (i = 0; i < 2; i++) {
		if ( zmq_msg_init(&zmsgBuff) < 0) {
			fprintf(stderr, "ERROR failed to init zmq msg buffer\n");
			goto err;
		}

		if ( zmq_msg_init_size(&zmsgBuff, BUFLEN) < 0) {
			fprintf(stderr, "ERROR failed to allocate memory for zmq msg buffer\n");
			goto err;
		}

		memset(zmq_msg_data(&zmsgBuff), 0, BUFLEN);
		//zmq_msg_set_group(&zmsgBuff, "GROUP1");
		if ( zmq_msg_recv(&zmsgBuff, zdish, 0) < 0) {
			fprintf(stderr, "ERROR failed to receive data\n");	
			/* FIXME - msg cleanup */
			goto err;
		}
		printf("RADIO> %s\n", (char *)zmq_msg_data(&zmsgBuff));
		zmq_msg_close(&zmsgBuff);
	}
	zmq_close (zdish);
	zmq_ctx_destroy (ctx);

	return 0;
err:
	zmq_close (zdish);
	zmq_ctx_destroy (ctx);
	exit(1);

}
