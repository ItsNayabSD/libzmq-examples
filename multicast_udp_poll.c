#define ZMQ_BUILD_DRAFT_API 1
#define IF_ADDR_LENGTH 32
#define MSG_SIZE 1024
#define GROUP "mygroup"

#include <zmq.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void mem_free(void *data, void *hint)
{
    free(data);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Specify multicast address along with port\n");
        printf("Ex: 239.0.0.2:51234\n");
        return argc;
    }
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the radio socket */
    void *radio = zmq_socket(ctx, ZMQ_RADIO);
    if (!radio) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Creating the dish socket */
    void *dish = zmq_socket(ctx, ZMQ_DISH);
    if (!dish) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* ret = zmq_join(dish, GROUP); */
    ret = zmq_join(dish, GROUP);

    /* Different ways to bind to local address */
    /* TCP port 5555 on all available interfaces (endpint address): */
    //     zmq_bind(radio, "tcp://*:5555");
    /* TCP port 5555 on the local loop-back interface on all platforms.
     *      zmq_bind(radio, "tcp://127.0.0.1:5555");
     * TCP port 5555 on the first Ethernet network interface on Linux.
     *      zmq_bind(radio, "tcp://eth0:5555");
     */

    char endpoint[IF_ADDR_LENGTH];
    memset(endpoint, 0, IF_ADDR_LENGTH);

    snprintf(endpoint, IF_ADDR_LENGTH, "udp://%s", argv[1]);
    /* listens to all ip address in a network on port 51234 */
    ret = zmq_connect(radio, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    ret = zmq_bind(dish, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }
    printf("Line %d\n", __LINE__);

    zmq_pollitem_t items [] = {
        { radio, 0, ZMQ_POLLOUT, 0 },
        { dish,  0, ZMQ_POLLIN, 0 }
    };
    int i = 0;
    while (1) {
    printf("Line %d\n", __LINE__);
        zmq_poll (items, 2, -1);
        /* Initializing the message */
        /* Three ways. */
        /* 1. int zmq_msg_init (&msg); Useful for the receiving end.
         * Initialize empty message*/
        /* 2. int zmq_msg_init_size (zmq_msg_t *msg, size_t size); */
        /* 3. int zmq_msg_init_data (zmq_msg_t *msg, void *data, size_t size, zmq_free_fn *ffn, void *hint); */

    printf("Line %d\n", __LINE__);
        if (items [0].revents & ZMQ_POLLOUT) {
            while (1) {
                zmq_msg_t msg;
                void *data = NULL;
                i++;
                data = malloc(MSG_SIZE);
                char hworld[MSG_SIZE];
                memset(hworld, 0, sizeof(hworld));
                snprintf(hworld, MSG_SIZE, "Hello World! - %d", i);
                memcpy(data, hworld, MSG_SIZE);

                /* NULL (or any type of data) is passed as a second argument to mem_free fn */
                ret = zmq_msg_init_data(&msg, data, MSG_SIZE, mem_free, NULL);
                if (ret) {
                    printf("Returned with %d at %d\n", errno, __LINE__);
                    return errno;
                }

                void *msg_ptr = NULL;
                /* zmq_msg_data() returns pointer to the content of zmq_msg_t object. */
                /* Use this pointer to write msg using memcpy */
                msg_ptr = zmq_msg_data(&msg);
                if (!msg_ptr) {
                    printf("zmq_msg_data failed at %d\n", __LINE__);
                    return EXIT_FAILURE;
                }

                /* Setting the group */
                ret = zmq_msg_set_group(&msg, GROUP);
                if (ret) {
                    printf("Returned with %d at %d\n", errno, __LINE__);
                    return errno;
                }

                /* Either use getchar() as a signal to start or use zmq_msg_send in while loop */
                /* Since zmq_msg_snd is lossy, dish might not receive message */
                sleep(1);
                /* getchar(); */

                printf("Sending message %s at %d\n", (char *)msg_ptr, __LINE__);
                /* Sending message */
                ret = zmq_msg_send(&msg, radio, 0);
                if (ret == -1) {
                    printf("Returned with %d at %d\n", errno, __LINE__);
                    return errno;
                }
                puts("Sent message");
                /* Deallocation msg object resources */
                ret = zmq_msg_close(&msg);
                if (ret == -1) {
                    printf("Returned with %d at %d\n", errno, __LINE__);
                    return errno;
                }
                break;

            }
        }
    printf("Line %d\n", __LINE__);
        if (items [1].revents & ZMQ_POLLIN) {
            printf("Line %d\n",__LINE__);

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
                break;
            }


        }
    }

    ret = zmq_disconnect(radio, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Stop accepting connections on a socket */
    ret = zmq_unbind(dish, endpoint);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }
    /* Closing the socket */
    ret = zmq_close(radio);
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
