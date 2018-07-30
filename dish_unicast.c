#define ZMQ_BUILD_DRAFT_API 1
#define IF_ADDR_LENGTH 32
#define GROUP "mygroup"

#include <zmq.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Specify multicast address along with port\n");
        printf("Multicast Ex: 239.0.0.2:51234\n");
        printf("Unicast: *:51717\n");
        return -1;
    }
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the socket */
    void *dish = zmq_socket(ctx, ZMQ_DISH);
    if (!dish) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* ret = zmq_join(dish, GROUP); */
    ret = zmq_join(dish, GROUP);

    /* Connecting using an IP address */
    char endpoint[32];

    memset(endpoint, 0, sizeof(endpoint));
    snprintf(endpoint, sizeof(endpoint), "udp://%s", argv[1]);
    ret = zmq_bind(dish, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }
    printf("Connected to %s\n",endpoint);

    /* Initialize the message */
    while (1) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);

        ret = zmq_msg_recv(&msg, dish, 0);
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
    }

    /* Stop accepting connections on a socket */
    ret = zmq_unbind(dish, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Closing the socket */
    ret = zmq_close(dish);
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
