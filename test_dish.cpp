#include "testutil.hpp"

int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_)
{
    int rc = zmq_msg_init (msg_);
    if (rc != 0)
        return -1;

    int recv_rc = zmq_msg_recv (msg_, s_, 0);
    if (recv_rc == -1)
        return -1;

    if (strcmp (zmq_msg_group (msg_), group_) != 0)
    {
        zmq_msg_close (msg_);
        return -1;
    }

    char * body = (char*) malloc (sizeof(char) * (zmq_msg_size (msg_) + 1));
    memcpy (body, zmq_msg_data (msg_), zmq_msg_size (msg_));
    body [zmq_msg_size (msg_)] = '\0';

    if (strcmp (body, body_) != 0)
    {
        zmq_msg_close (msg_);
        return -1;
    }

    zmq_msg_close (msg_);
    free(body);
    return recv_rc;
}

int main (void)
{
    // size_t len = MAX_SOCKET_STRING;
    char my_endpoint[MAX_SOCKET_STRING];
    setup_test_environment ();
    void *ctx = zmq_ctx_new ();
    assert (ctx);

    void *dish = zmq_socket (ctx, ZMQ_DISH);
    int rc;

    // Joining
    rc = zmq_join (dish, "Movies");
    assert (rc == 0);

    // Duplicate Joining
    rc = zmq_join (dish, "Movies");
    assert (rc == -1);

    // Connecting
    rc = zmq_connect (dish,"tcp://127.0.0.1:33333");
    assert (rc == 0);

    msleep (SETTLE_TIME);

    zmq_msg_t msg;

    //  Check the correct message arrived
    rc = msg_recv_cmp (&msg, dish, "Movies", "Godfather");
    assert (rc == 9);

    zmq_sleep (1);

    rc = zmq_close (dish);
    assert (rc == 0);

    rc = zmq_ctx_term (ctx);
    assert (rc == 0);

    return 0 ;
}
