#include "oneshot.h"
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "eventthread.h"

#include <SDL.h>
#include <boost/crc.hpp>

RB_METHOD(LoadUserData) {
	char* path
	rb_get_args(argc, argv, "z", &path);
	return Qnil
}

void modshotgamejoltBindingInit()
{
	VALUE module = rb_define_module("Gamejolt");
	VALUE msg = rb_define_module_under(module, "Msg");
	_rb_define_module_function(Gamejolt, "LoadUserData", "LoadUserData)
	//_rb_define_module_function(module, "defhere", exposedname);
}