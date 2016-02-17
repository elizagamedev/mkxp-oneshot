#ifdef STEAM
#include "steam.h"
#endif
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "debugwriter.h"

#ifdef STEAM
static ID achievementIds[Achievement::MAX];

static Achievement::ID toAchievementId(ID id)
{
	/* Crude but effective */
	for (int i = 0; i < Achievement::MAX; ++i)
	{
		if (achievementIds[i] == id)
			return static_cast<Achievement::ID>(i);
	}
	return Achievement::Invalid;
}
#endif

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

    ID rbid;
	rb_get_args(argc, argv, "n", &rbid RB_ARG_END);

#ifdef STEAM
	Achievement::ID id = toAchievementId(rbid);
	if (id == Achievement::Invalid)
		rb_raise(rb_eNameError, "unlocked nonexistent achievement");
	else
		shState->steam().unlock(id);
#endif
	return Qnil;
}

RB_METHOD(steamLock)
{
	RB_UNUSED_PARAM;

    ID rbid;
	rb_get_args(argc, argv, "n", &rbid RB_ARG_END);

#ifdef STEAM
	Achievement::ID id = toAchievementId(rbid);
	if (id == Achievement::Invalid)
		rb_raise(rb_eNameError, "locked nonexistent achievement");
	else
		shState->steam().lock(id);
#endif
	return Qnil;
}

RB_METHOD(steamUnlocked)
{
	RB_UNUSED_PARAM;

	ID rbid;
	rb_get_args(argc, argv, "n", &rbid RB_ARG_END);

#ifdef STEAM
	Achievement::ID id = toAchievementId(rbid);
	if (id == Achievement::Invalid)
		return Qfalse;
	return shState->steam().isUnlocked(id) ? Qtrue : Qfalse;
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

#ifdef STEAM
	/* Cache achievement ids */
	for (int i = 0; i < Achievement::MAX; ++i)
		achievementIds[i] = rb_intern(Achievement::names[i]);
#endif
}
