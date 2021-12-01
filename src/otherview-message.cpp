#include "otherview-message.h"
#include "config.h"
#include "debugwriter.h"

#include <zmq.hpp>
#include <zmq_addon.hpp>

OtherViewMessager::OtherViewMessager(const Config &c):
    conf(c)
{
    if (conf.isOtherView) { 
        otherview = zmq::socket_t (ctx, ZMQ_PUSH);
        mainwindow = zmq::socket_t (ctx, ZMQ_PULL);
    } else {
        otherview = zmq::socket_t (ctx, ZMQ_PULL);
        mainwindow = zmq::socket_t (ctx, ZMQ_PUSH);
    }

    otherview.connect("tcp://localhost:9698");
    mainwindow.connect("tcp://loclahost:9697");
}

void OtherViewMessager::sendMsg(const char *str) {
    size_t size = strlen(str); // Assuming your char* is NULL-terminated
    zmq::message_t message(size);
    std::memcpy (message.data(), str, size);

    if (conf.isOtherView) {
        otherview.send (message, zmq::send_flags::none);
    } else {
        mainwindow.send (message, zmq::send_flags::none);
    }
}

const char* OtherViewMessager::getMsg() {
    zmq::message_t message;
    if (conf.isOtherView) {
        auto res = mainwindow.recv (message, zmq::recv_flags::dontwait);

        if (res.has_value() && (EAGAIN == res.value())) {
            return "NO MESSAGES";
        } else  {
            return (char*) message.data();
        }
    } else {
        auto res = otherview.recv (message, zmq::recv_flags::dontwait);

        if (res.has_value() && (EAGAIN == res.value())) {
            return "NO MESSAGES";
        } else  {
            return (char*) message.data();
        }
    }

}
// Ignore because 0MQ has some python zen ass bullshit and you can't check the amount of messages in a queue
// I also can't figure out how to enable the active poller which would bypass this problem entirely
int OtherViewMessager::unreadMsgs() {
    return 0;
}