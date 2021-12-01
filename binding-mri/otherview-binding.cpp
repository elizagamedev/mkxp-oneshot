#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "otherview-message.h"

#include <vector>

std::vector<VALUE> subscribers;

void notify(VALUE message)
{
    std::vector<VALUE>::iterator it;

    for (it = subscribers.begin(); it != subscribers.end(); it++)
    {
        rb_proc_call(*it, message);
    }
}

RB_METHOD(pushSubscriber)
{
    VALUE subscriberProc;
    rb_get_args(argc, argv, "o", &subscriberProc RB_ARG_END);

    subscribers.push_back(subscriberProc);

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
    const char* response = messager.getMsg();
    if (response !=  "NO MESSAGES") {
        notify(rb_str_new_cstr(response));
    }

    return Qnil;
}

RB_METHOD(sendMessage)
{
    OtherViewMessager &messager = shState->otherView();

    const char* message;
    rb_get_args(argc, argv, "z", &message RB_ARG_END);

    messager.sendMsg(message);

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
}