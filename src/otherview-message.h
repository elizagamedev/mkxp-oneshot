#include "etc.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>

struct Config;

class OtherViewMessager {
    zmq::socket_t otherview;
    zmq::socket_t mainwindow;
    zmq::context_t ctx;
    const Config &conf;
    
public:
    OtherViewMessager(const Config &c);
    void update();
    void sendMsg(const char *str);
    const char* getMsg();
    int unreadMsgs();
    void closeSockets();
};