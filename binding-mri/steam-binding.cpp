#ifdef STEAM
#include "steam.h"
#endif
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "debugwriter.h"

RB_METHOD(steamEnabled)
{
	RB_UNUSED_PARAM;

#ifdef STEAM
	return Qtrue;
#else
	return Qfalse;
#endif
}

RB_METHOD(steamUnlock)
{
	RB_UNUSED_PARAM;

    const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

#ifdef STEAM
	shState->steam().unlock(name);
#endif
	return Qnil;
}

RB_METHOD(steamLock)
{
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

#ifdef STEAM
	shState->steam().lock(name);
#endif
	return Qnil;
}

RB_METHOD(steamUnlocked)
{
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

#ifdef STEAM
	return shState->steam().isUnlocked(name) ? Qtrue : Qfalse;
#else
	return Qfalse;
#endif
}

void steamBindingInit()
{
    VALUE module = rb_define_module("Steam");

	/* Constants */
#ifdef STEAM
	rb_const_set(module, rb_intern("USER_NAME"), rb_str_new2(shState->steam().userName().c_str()));
#else
	rb_const_set(module, rb_intern("USER_NAME"), Qnil);
#endif

	/* Functions */
	_rb_define_module_function(module, "enabled?", steamEnabled);
    _rb_define_module_function(module, "unlock", steamUnlock);
	_rb_define_module_function(module, "lock", steamLock);
	_rb_define_module_function(module, "unlocked?", steamUnlocked);
}
