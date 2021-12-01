#include "etc.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>

struct Config;

class OtherViewMessager {
    zmq::context_t ctx;
    
    zmq::socket_t otherview;
    zmq::socket_t mainwindow;

    const Config &conf;
    
public:
    OtherViewMessager(const Config &c);
    void update();
    void sendMsg(const char *str);
    const char* getMsg();
    int unreadMsgs();
};