#define ZMQ_BUILD_DRAFT_API 1

#include <zmq.h>
#include <errno.h>

int main(void)
{
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the socket */
    void *socket = zmq_socket(ctx, ZMQ_DISH);
    if (!socket) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Different ways to connect to end point */
    /* Connecting using an IP address */
    /*      zmq_connect(socket, "tcp://192.168.1.1:5555"); */
    /* FIXME: Connecting using a DNS name (Not done yet)*/
    /*      zmq_connect(socket, "tcp://server1:5555"); */
    /* FIXME: Connecting using a DNS name and bind to eth1 (Not done yet) */
    /*      zmq_connect(socket, "tcp://eth1:0;server1:5555"); */
    /* FIXME: Connecting using a IP address and bind to an IP address (Not done yet) */
    /*      zmq_connect(socket, "tcp://192.168.1.17:5555;192.168.1.1:5555"); assert (rc == 0); */

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
