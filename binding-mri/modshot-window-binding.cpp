#include "oneshot.h"
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "eventthread.h"

#include <SDL.h>
#include <SDL_image.h>
#include <boost/crc.hpp>

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
	char* wintitle; //thx rkevin
	rb_get_args(argc, argv, "z", &wintitle);
	SDL_SetWindowTitle(shState->rtData().window, wintitle);
	return Qnil;
}

RB_METHOD(SetIcon) {
	char* path;
	rb_get_args(argc, argv, "z", &path);
	SDL_Surface* icon = IMG_Load(path);
	if (!icon) {
		rb_raise(rb_eRuntimeError, "Loading icon from path failed");
	}
	SDL_SetWindowIcon(shState->rtData().window, icon);
	return Qnil;
}

void modshotwindowBindingInit()
{
	VALUE module = rb_define_module("ModWindow");
	_rb_define_module_function(module, "GetWindowPosition", GetWindowPosition);
	_rb_define_module_function(module, "SetWindowPosition", SetWindowPosition);
	_rb_define_module_function(module, "SetTitle", SetTitle);
	_rb_define_module_function(module, "SetIcon", SetIcon);
}
