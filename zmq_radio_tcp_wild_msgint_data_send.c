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
        printf("Specify port as an argument\n");
        return argc;
    }
    void *ctx = zmq_ctx_new();
    int ret;

    /* Creating the socket */
    void *socket = zmq_socket(ctx, ZMQ_RADIO);
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

    char end_point[IF_ADDR_LENGTH];
    memset(end_point, 0, IF_ADDR_LENGTH);

    snprintf(end_point, IF_ADDR_LENGTH, "tcp://0.0.0.0:%s", argv[1]);
    /* listens to all ip address in a network on port 51234 */
    ret = zmq_bind(socket, end_point);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Initializing the message */
    /* Three ways. */
    /* 1. int zmq_msg_init (&msg); Useful for the receiving end.
     * Initialize empty message*/
    /* 2. int zmq_msg_init_size (zmq_msg_t *msg, size_t size); */
    /* 3. int zmq_msg_init_data (zmq_msg_t *msg, void *data, size_t size, zmq_free_fn *ffn, void *hint); */

    zmq_msg_t msg;
    void *data = NULL;

    data = malloc(MSG_SIZE);
    memcpy(data, "Hello World!", sizeof("Hello World!"));

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
    puts("Press enter when you are ready");
    getchar();

    printf("Sending message %s at %d\n", (char *)msg_ptr, __LINE__);
    /* Sending message */
    ret = zmq_msg_send(&msg, socket, 0);
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

    /* Finding associated IP address and port*/
    char if_address[IF_ADDR_LENGTH];
    size_t if_length = IF_ADDR_LENGTH;

    memset(if_address, 0, sizeof(if_address));
    ret = zmq_getsockopt(socket, ZMQ_LAST_ENDPOINT, if_address, &if_length);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Stop accepting connections on a socket */
    ret = zmq_unbind(socket, if_address);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* Closing the socket */
    ret = zmq_close(socket);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return errno;
    }

    /* To free resources (context) allocated by zmq */
    ret = zmq_ctx_term(ctx);
    if (ret) {
        printf("Returned with %d at %d\n", errno, __LINE__);
        return ret;
    }
    return 0;
}
