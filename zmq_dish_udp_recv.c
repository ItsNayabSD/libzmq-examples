#define ZMQ_BUILD_DRAFT_API 1
#define GROUP "mygroup"

#include <zmq.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        puts("Pass IP address as argument along with port");
        puts("Example (In the same system): ./exe 127.0.0.1:51234 or ./exe 0.0.0.0:51234");
        puts("Example (For different systems): ./exe 192.168.101.202:51234 (Need to test)");
        return -1;
    }
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the socket */
    void *socket = zmq_socket(ctx, ZMQ_DISH);
    if (!socket) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* ret = zmq_join(socket, GROUP); */
    ret = zmq_join(socket, GROUP);

    /* Connecting using an IP address */
    char endpoint[32];

    memset(endpoint, 0, sizeof(endpoint));
    snprintf(endpoint, sizeof(endpoint), "udp://%s", argv[1]);
    ret = zmq_bind(socket, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }
    printf("Connected to %s\n",endpoint);

    /* Initialize the message */
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    ret = zmq_msg_recv(&msg, socket, 0);
    if (ret == -1) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    char * body = (char*) malloc (sizeof(char) * (zmq_msg_size (&msg) + 1));
    memcpy (body, zmq_msg_data (&msg), zmq_msg_size (&msg));
    body [zmq_msg_size (&msg)] = '\0';

    printf("Received the msg %s\n", body);

    /* Deallocation msg object resources */
    ret = zmq_msg_close(&msg);
    if (ret == -1) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    ret = zmq_disconnect(socket, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Closing the socket */
    ret = zmq_close(socket);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }

    /* To free resources (context) allocated by zmq */
    ret = zmq_ctx_term(ctx);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }
    return 0;
}
