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
