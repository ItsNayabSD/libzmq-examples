#define ZMQ_BUILD_DRAFT_API 1

#include <zmq.h>
#include <errno.h>

int main(void)
{
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the socket */
    void *socket = zmq_socket(ctx, ZMQ_SERVER);
    if (!socket) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Different ways to bind to local address */
    /* TCP port 5555 on all available interfaces (endpint address): */
    //     zmq_bind(socket, "tcp://*:5555");
    /* TCP port 5555 on the local loop-back interface on all platforms.
     *      zmq_bind(socket, "tcp://127.0.0.1:5555");
     * TCP port 5555 on the first Ethernet network interface on Linux.
     *      zmq_bind(socket, "tcp://eth0:5555");
     */

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
