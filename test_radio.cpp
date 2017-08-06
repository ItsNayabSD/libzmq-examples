#include "testutil.hpp"

int msg_send (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_)
{
    int rc = zmq_msg_init_size (msg_, strlen (body_));
    if (rc != 0)
        return rc;

    memcpy (zmq_msg_data (msg_), body_, strlen (body_));

    rc = zmq_msg_set_group (msg_, group_);
    if (rc != 0) {
        zmq_msg_close (msg_);
        return rc;
    }

    rc = zmq_msg_send (msg_, s_, 0);

    zmq_msg_close (msg_);

    return rc;
}

int main (void)
{
    setup_test_environment ();
    void *ctx = zmq_ctx_new ();
    assert (ctx);

    void *radio = zmq_socket (ctx, ZMQ_RADIO);

    int rc = zmq_bind (radio, "tcp://127.0.0.1:33333");
    assert (rc == 0);

    zmq_msg_t msg;

    //  This is going to be sent to the dish
    rc = msg_send (&msg, radio, "Movies", "Godfather");
    assert (rc == 9);

    rc = zmq_close (radio);
    assert (rc == 0);

    rc = zmq_ctx_term (ctx);
    assert (rc == 0);

    return 0 ;
}
