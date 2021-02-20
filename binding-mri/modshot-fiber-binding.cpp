#include "binding-util.h"

RB_METHOD(modshot_rb_fiber_alive_p) {
    return rb_fiber_alive_p(self);
}

void modshotFiberBindingInit() {
    VALUE rb_cFiber = rb_define_class("Fiber", rb_cObject);
    _rb_define_method(rb_cFiber, "alive?", modshot_rb_fiber_alive_p);
}
