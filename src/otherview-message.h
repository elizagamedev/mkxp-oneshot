#pragma once

#include "etc.h"
#include <string>

struct Config;

class OtherViewMessager {
    const Config &conf;
    
public:
    OtherViewMessager(const Config &c);
    void update();
    void sendMsg(std::string str);
    std::string getMsg();
    int unreadMsgs();
    void closeSockets();

    void OpenOneShot();
};