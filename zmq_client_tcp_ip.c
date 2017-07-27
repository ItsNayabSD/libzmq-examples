#define ZMQ_BUILD_DRAFT_API 1

#include <zmq.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        puts("Pass IP address as argument");
        return -1;
    }
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the socket */
    void *socket = zmq_socket(ctx, ZMQ_CLIENT);
    if (!socket) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Connecting using an IP address */
    char endpoint[32];

    memset(endpoint, 0, sizeof(endpoint));
    snprintf(endpoint, sizeof(endpoint), "tcp://%s:5555", argv[1]);
    ret = zmq_connect(socket, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }

    ret = zmq_disconnect(socket, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
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
