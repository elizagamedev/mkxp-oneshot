#include "oneshot.h"
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "eventthread.h"

#include <SDL.h>

RB_METHOD(oneshotSetYesNo)
{
    RB_UNUSED_PARAM;

    const char *yes;
    const char *no;
    rb_get_args(argc, argv, "zz", &yes, &no RB_ARG_END);
    shState->oneshot().setYesNo(yes, no);
    return Qnil;
}

RB_METHOD(oneshotMsgBox)
{
    RB_UNUSED_PARAM;

    int type;
    VALUE body;
    VALUE title = Qnil;
    rb_get_args(argc, argv, "iS|S", &type, &body, &title RB_ARG_END);
	std::string bodyStr = std::string(RSTRING_PTR(body), RSTRING_LEN(body));
	std::string titleStr = (title == Qnil) ? "" : std::string(RSTRING_PTR(title), RSTRING_LEN(title));
    return rb_bool_new(shState->oneshot().msgbox(type, bodyStr.c_str(), titleStr.c_str()));
}

RB_METHOD(oneshotResetObscured)
{
	RB_UNUSED_PARAM;
	shState->oneshot().resetObscured();
	return Qnil;
}

RB_METHOD(oneshotObscuredCleared)
{
	RB_UNUSED_PARAM;
	return shState->oneshot().obscuredCleared() ? Qtrue : Qfalse;
}

RB_METHOD(oneshotAllowExit)
{
	RB_UNUSED_PARAM;
	bool allowExit;
	rb_get_args(argc, argv, "b", &allowExit RB_ARG_END);
	shState->oneshot().setAllowExit(allowExit);
	return Qnil;
}

RB_METHOD(oneshotShake)
{
	RB_UNUSED_PARAM;
	int absx, absy;
	SDL_GetWindowPosition(shState->rtData().window, &absx, &absy);
	srand(time(NULL));
	for (int i = 0; i < 60; ++i) {
		int max = 60 - i;
		int x = rand() % (max * 2) - max;
		int y = rand() % (max * 2) - max;
		SDL_SetWindowPosition(shState->rtData().window, absx + x, absy + y);
	}
	return Qnil;
}

RB_METHOD(oneshotmovewindow)
{
	RB_UNUSED_PARAM;
	int x, y;
	rb_get_args(argc, argv, "ii", &x, &y RB_ARG_END);
	SDL_SetWindowPosition(shState->rtData().window, x, y)
	return Qnil
}

RB_METHOD(oneshotgetwindowpos)
{
	RB_UNUSED_PARAM;
	int x, y;
	SDL_GetWindowPosition(shState->rtData().window, &x, &y);
	VALUE ary = rb_ary_new;
	rb_ary_push(ary,x);
	rb_ary_push(ary,y);
	return ary
}

void oneshotBindingInit()
{
    VALUE module = rb_define_module("Oneshot");
    VALUE msg = rb_define_module_under(module, "Msg");

    //Constants
    rb_const_set(module, rb_intern("USER_NAME"), rb_str_new2(shState->oneshot().userName().c_str()));
    rb_const_set(module, rb_intern("SAVE_PATH"), rb_str_new2(shState->oneshot().savePath().c_str()));
	rb_const_set(module, rb_intern("DOCS_PATH"), rb_str_new2(shState->oneshot().docsPath().c_str()));
	rb_const_set(module, rb_intern("GAME_PATH"), rb_str_new2(shState->oneshot().gamePath().c_str()));
	rb_const_set(module, rb_intern("JOURNAL"), rb_str_new2(shState->oneshot().journal().c_str()));
    rb_const_set(module, rb_intern("LANG"), rb_str_new2(shState->oneshot().lang().c_str()));
    rb_const_set(msg, rb_intern("INFO"), INT2FIX(Oneshot::MSG_INFO));
    rb_const_set(msg, rb_intern("YESNO"), INT2FIX(Oneshot::MSG_YESNO));
    rb_const_set(msg, rb_intern("WARN"), INT2FIX(Oneshot::MSG_WARN));
    rb_const_set(msg, rb_intern("ERR"), INT2FIX(Oneshot::MSG_ERR));

    //Functions
    _rb_define_module_function(module, "set_yes_no", oneshotSetYesNo);
    _rb_define_module_function(module, "msgbox", oneshotMsgBox);
	_rb_define_module_function(module, "reset_obscured", oneshotResetObscured);
	_rb_define_module_function(module, "obscured_cleared?", oneshotObscuredCleared);
	_rb_define_module_function(module, "allow_exit", oneshotAllowExit);
	_rb_define_module_function(module, "shake", oneshotShake);
}
