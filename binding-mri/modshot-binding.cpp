#include "oneshot.h"
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "eventthread.h"

#include <SDL.h>
#include <boost/crc.hpp>

RB_METHOD(bounce_up)
{
	//Edit: this is useless garbage that doesn't work
	int absx, absy;
	SDL_GetWindowPosition(shState->rtData().window, &absx, &absy);
	int state;
	for (int i = 0; i < 60; ++i) {
		int max = 60 - i;
		int y = -5;
		int x = 0;
		SDL_SetWindowPosition(shState->rtData().window, absx + x, absy + y);
		rb_eval_string_protect("sleep 0.02", &state);
	}
	for (int i = 0; i < 60; ++i) {
		int max = 60 - i;
		int y = 5;
		int x = 0;
		SDL_SetWindowPosition(shState->rtData().window, absx + x, absy + y);
		rb_eval_string_protect("sleep 0.02", &state);
	}
	return Qnil;
}

//The window x and y values are halved for some reason, so we need to double them
RB_METHOD(GetWindowPosition) {
	int x, y;
	SDL_GetWindowPosition(shState->rtData().window, &x, &y);
	return rb_ary_new3(2, LONG2FIX(x), LONG2FIX(y));
}

RB_METHOD(SetWindowPosition) {
	int x, y;
	rb_get_args(argc, argv, "ii", &x, &y);
	SDL_SetWindowPosition(shState->rtData().window, x, y);
	return Qnil;
}

RB_METHOD(SetTitle) {
	char* wintitle; //i need to look at cpp tutorials
	rb_get_args(argc, "z", &wintitle);
	SetWindowText(0, wintitle);
	return Qnil;
}

void modshotBindingInit()
{
	VALUE module = rb_define_module("Modshot");
	VALUE msg = rb_define_module_under(module, "Msg");
	_rb_define_module_function(module, "bounce_up", bounce_up);
	//_rb_define_module_function(module, "defhere", exposedname);
	_rb_define_module_function(module, "GetWindowPosition", GetWindowPosition);
	_rb_define_module_function(module, "SetWindowPosition", SetWindowPosition);
	_rb_define_module_function(module, "SetTitle", SetTitle);
}