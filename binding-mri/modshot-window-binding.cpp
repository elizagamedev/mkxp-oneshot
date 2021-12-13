#include "oneshot.h"
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "eventthread.h"

#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_syswm.h>
#include <boost/crc.hpp>

#ifdef _WIN32
	#include <windows.h>
#else
	//#include 
#endif

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

/*
RB_METHOD(SetTransparentColor) {
	
	RB_UNUSED_PARAM;
	Value colorObj;

	rb_get_args(argc, argv, "o", &colorObj RB_ARG_END);

	Color *c = getPrivateDataCheck<Color>(colorObj, ColorType);

	#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
    SDL_GetWindowWMInfo(shState->rtData().window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    // Set transparency color
    SetLayeredWindowAttributes(hWnd, RGB(c.r, c.g, c.b), 0, LWA_COLORKEY);
	#else

	#endif
	return Qnil;
}
*/

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

RB_METHOD(SetWindowOpacity) {

	float opacity;
	rb_get_args(argc, argv, "f", &opacity);
	SDL_SetWindowOpacity(shState->rtData().window, opacity);
	return Qnil;
}

void modshotwindowBindingInit()
{
	VALUE module = rb_define_module("ModWindow");
	_rb_define_module_function(module, "GetWindowPosition", GetWindowPosition);
	_rb_define_module_function(module, "SetWindowPosition", SetWindowPosition);
	_rb_define_module_function(module, "SetTitle", SetTitle);
	_rb_define_module_function(module, "SetIcon", SetIcon);
	_rb_define_module_function(module, "setWindowOpacity", SetWindowOpacity);
	//_rb_define_module_function(module, "setWindowChromaKey", SetTransparentColor);
}
