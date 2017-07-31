/* Zero MQ UDP Multicast Radio (Sender / Broadcaster) */


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
	int n;
	struct hostent *server;

	char radio_addr[RADIO_ADDR_LEN] = {0};	
	zmq_msg_t zmsgBuff;
	void *msgBuff;

	void *ctx = zmq_ctx_new(); /* Any zmq socket requires a context */
	void *zradio = zmq_socket(ctx, ZMQ_RADIO);

	if (zradio == NULL) {
		perror("Error: ");
		exit(1);
	}

#if 0
	if (argc != NUMARGS) {
		fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
		exit(1);
	}
	
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR no such host\n");
		exit(1);
	}
#endif
	/* FIXME - get the address and port as input */
	if ( zmq_connect(zradio, "udp://127.0.0.1:51717") < 0) {
		fprintf(stderr, "ERROR failed to connect zmq socket\n");			
		goto err;
	}


	if ( zmq_msg_init(&zmsgBuff) < 0) {
		fprintf(stderr, "ERROR failed to init zmq msg buffer\n");
		goto err;
	}


	if ( zmq_msg_init_size(&zmsgBuff, BUFLEN) < 0) {
		fprintf(stderr, "ERROR failed to allocate memory for zmq msg buffer\n");
		goto err;
	}

	msgBuff = malloc(BUFLEN);

	if (msgBuff == NULL) {
		fprintf(stderr, "ERROR failed to allocate memory for msg buffer\n");		
		goto err; /*FIXME - add free for zmq msg buffer */
	}

	bzero (msgBuff, sizeof(msgBuff));
	memcpy(msgBuff, "HelloWorld !", 12);

	if ( zmq_msg_init_data (&zmsgBuff, msgBuff, 12, zmsgFree, NULL) < 0) {
		fprintf(stderr, "ERROR failed to initialize zmq msg buffer with data\n");		
		goto err; /*FIXME - add free for zmq msg buffer */
	}

	zmq_msg_set_group(&zmsgBuff, "Hostapd");
	if ( zmq_msg_send(&zmsgBuff, zradio, 0) < 0) {
		perror("Error: ");
		/* FIXME - msg cleanup */
		goto err;
	}


	zmq_msg_set_group(&zmsgBuff, "Wifidog");
	if ( zmq_msg_send(&zmsgBuff, zradio, 0) < 0) {
		perror("Error: ");
		/* FIXME - msg cleanup */
		goto err;
	}

	zmq_close (zradio);
	zmq_ctx_destroy (ctx);

	return 0;
err:
	zmq_close (zradio);
	zmq_ctx_destroy (ctx);
	exit(1);

}
