#include "oneshot.h"
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "eventthread.h"

#include <SDL.h>
#include <boost/crc.hpp>

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

RB_METHOD(oneshotTextInput)
{
	RB_UNUSED_PARAM;
	VALUE prompt;
	int char_limit = 100;
	VALUE font = Qnil;
	rb_get_args(argc, argv, "S|iS", &prompt, &char_limit, &font RB_ARG_END);
	std::string promptStr = std::string(RSTRING_PTR(prompt), RSTRING_LEN(prompt));
	std::string fontStr = (font == Qnil) ? "" : std::string(RSTRING_PTR(font), RSTRING_LEN(font));
	return rb_str_new2(shState->oneshot().textinput(promptStr.c_str(), char_limit, fontStr.c_str()).c_str());
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

RB_METHOD(oneshotExiting)
{
	RB_UNUSED_PARAM;
	bool exiting;
	rb_get_args(argc, argv, "b", &exiting RB_ARG_END);
	shState->oneshot().setExiting(exiting);
	return Qnil;
}

RB_METHOD(oneshotShake)
{
	RB_UNUSED_PARAM;
	int absx, absy;
	SDL_GetWindowPosition(shState->rtData().window, &absx, &absy);
	int state;
	srand(time(NULL));
	for (int i = 0; i < 60; ++i) {
		int max = 60 - i;
		int x = rand() % (max * 2) - max;
		int y = rand() % (max * 2) - max;
		SDL_SetWindowPosition(shState->rtData().window, absx + x, absy + y);
		rb_eval_string_protect("sleep 0.02", &state);
	}
	return Qnil;
}

RB_METHOD(oneshotCRC32)
{
	RB_UNUSED_PARAM;
	VALUE string;
	boost::crc_32_type result;
	rb_get_args(argc, argv, "S", &string RB_ARG_END);
	std::string str = std::string(RSTRING_PTR(string), RSTRING_LEN(string));
	result.process_bytes(str.data(), str.length());
	return UINT2NUM(result.checksum());
}

void oneshotBindingInit()
{
	VALUE module = rb_define_module("Oneshot");
	VALUE msg = rb_define_module_under(module, "Msg");

	// Constants
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

	// Functions
	_rb_define_module_function(module, "set_yes_no", oneshotSetYesNo);
	_rb_define_module_function(module, "msgbox", oneshotMsgBox);
	_rb_define_module_function(module, "textinput", oneshotTextInput);
	_rb_define_module_function(module, "reset_obscured", oneshotResetObscured);
	_rb_define_module_function(module, "obscured_cleared?", oneshotObscuredCleared);
	_rb_define_module_function(module, "allow_exit", oneshotAllowExit);
	_rb_define_module_function(module, "exiting", oneshotExiting);
	_rb_define_module_function(module, "shake", oneshotShake);
	_rb_define_module_function(module, "crc32", oneshotCRC32);
}
