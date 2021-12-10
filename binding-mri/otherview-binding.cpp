#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "otherview-message.h"
#include "debugwriter.h"

#include <vector>
#include <string>

std::vector<VALUE> subscribers;

void notify(VALUE message)
{
    /*
    Debug() << "ruby_string";
    Debug() << message;
    */
    VALUE args[] = { message };

    for (auto & element : subscribers) {
        /* rb_funcallv(element, rb_intern("call"), 1, args); */
        rb_funcallv(element, rb_intern("call"), 1, args);
    }
}

RB_METHOD(pushSubscriber)
{
    
    VALUE subscriberProc;
    rb_get_args(argc, argv, "o", &subscriberProc RB_ARG_END);
    subscribers.push_back(subscriberProc);
    /*
    VALUE obj = rb_block_proc();
    subscribers.push_back(obj);
    */
    return Qnil;
}

RB_METHOD(popSubscriber)
{
    int index = -1;
    rb_get_args(argc, argv, "|i", &index RB_ARG_END);
    if (index < 0) {
        subscribers.pop_back();
    } else {
        subscribers.erase(subscribers.begin() + index);
    }
    return Qnil;
}

RB_METHOD(returnSubscribers)
{
    VALUE returnValue = rb_ary_new();
    std::vector<VALUE>::iterator it;

    for (it = subscribers.begin(); it != subscribers.end(); it++)
    {
        rb_ary_push(returnValue, *it);
    }

    return returnValue;
}

RB_METHOD(otherViewUpdate)
{
    OtherViewMessager &messager = shState->otherView();
    std::string response = messager.getMsg();
    /*
    Debug() << "response";
    Debug() << response;
    Debug() << strlen(response);
    */
    
    VALUE str = rb_str_new_cstr(response.c_str());

    return str;
}

RB_METHOD(sendMessage)
{
    OtherViewMessager &messager = shState->otherView();

    std::string message;
    rb_get_args(argc, argv, "z", &message RB_ARG_END);

    messager.sendMsg(message);

    return Qnil;
}

RB_METHOD(openOneShot) 
{
    OtherViewMessager &messager = shState->otherView();
    messager.OpenOneShot();

    return Qnil;
}

void otherviewBindingInit()
{
    VALUE module = rb_define_module("OtherView");
    _rb_define_module_function(module, "pushSubscriber", pushSubscriber);
    _rb_define_module_function(module, "popSubscriber", popSubscriber);
    _rb_define_module_function(module, "subscribers", returnSubscribers);
    _rb_define_module_function(module, "update", otherViewUpdate);
    _rb_define_module_function(module, "send", sendMessage);
    _rb_define_module_function(module, "openOneShot", openOneShot);
}