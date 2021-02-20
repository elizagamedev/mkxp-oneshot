/*
** audio-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "audio.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "exception.h"

#define DEF_PLAY_STOP_POS(entity) \
	RB_METHOD(audio_##entity##Play) \
	{ \
		RB_UNUSED_PARAM; \
		const char *filename; \
		int volume = 100; \
		int pitch = 100; \
		double pos = -1.0; \
		bool fadeInOnOffset = true; \
		rb_get_args(argc, argv, "z|iifb", &filename, &volume, &pitch, &pos, &fadeInOnOffset RB_ARG_END); \
		GUARD_EXC( shState->audio().entity##Play(filename, volume, pitch, pos, fadeInOnOffset); ) \
		return Qnil; \
	} \
	RB_METHOD(audio_##entity##Stop) \
	{ \
		RB_UNUSED_PARAM; \
		shState->audio().entity##Stop(); \
		return Qnil; \
	} \
	RB_METHOD(audio_##entity##Pos) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_float_new(shState->audio().entity##Pos()); \
	}

#define DEF_PLAY_STOP(entity) \
	RB_METHOD(audio_##entity##Play) \
	{ \
		RB_UNUSED_PARAM; \
		const char *filename; \
		int volume = 100; \
		int pitch = 100; \
		rb_get_args(argc, argv, "z|ii", &filename, &volume, &pitch RB_ARG_END); \
		GUARD_EXC( shState->audio().entity##Play(filename, volume, pitch); ) \
		return Qnil; \
	} \
	RB_METHOD(audio_##entity##Stop) \
	{ \
		RB_UNUSED_PARAM; \
		shState->audio().entity##Stop(); \
		return Qnil; \
	}

#define DEF_FADE(entity) \
RB_METHOD(audio_##entity##Fade) \
{ \
	RB_UNUSED_PARAM; \
	int time; \
	rb_get_args(argc, argv, "i", &time RB_ARG_END); \
	shState->audio().entity##Fade(time); \
	return Qnil; \
}

#define DEF_POS(entity) \
	RB_METHOD(audio_##entity##Pos) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_float_new(shState->audio().entity##Pos()); \
	}

#define DEF_ISPLAYING(entity) \
	RB_METHOD(audio_##entity##IsPlaying) \
	{ \
		RB_UNUSED_PARAM; \
		return shState->audio().entity##IsPlaying() ? Qtrue : Qfalse; \
	}

#define DEF_AUD_PROP_I(PropName) \
	RB_METHOD(audio##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_fix_new(shState->audio().get##PropName()); \
	} \
	RB_METHOD(audio##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		int value; \
		rb_get_args(argc, argv, "i", &value RB_ARG_END); \
		shState->audio().set##PropName(value); \
		return rb_fix_new(value); \
	}

DEF_PLAY_STOP_POS( bgm )
DEF_PLAY_STOP_POS( bgs )

DEF_PLAY_STOP( me )

DEF_FADE( bgm )
DEF_FADE( bgs )
DEF_FADE( me )

DEF_ISPLAYING( bgm )
DEF_ISPLAYING( bgs )
DEF_ISPLAYING( me )

DEF_PLAY_STOP( se )

DEF_AUD_PROP_I(BGM_Volume)
DEF_AUD_PROP_I(SFX_Volume)

RB_METHOD(audioReset)
{
	RB_UNUSED_PARAM;

	shState->audio().reset();

	return Qnil;
}


#define BIND_PLAY_STOP(entity) \
	_rb_define_module_function(module, #entity "_play", audio_##entity##Play); \
	_rb_define_module_function(module, #entity "_stop", audio_##entity##Stop);

#define BIND_FADE(entity) \
	_rb_define_module_function(module, #entity "_fade", audio_##entity##Fade);

#define BIND_PLAY_STOP_FADE(entity) \
	BIND_PLAY_STOP(entity) \
	BIND_FADE(entity)

#define BIND_POS(entity) \
	_rb_define_module_function(module, #entity "_pos", audio_##entity##Pos);

#define BIND_IS_PLAYING(entity) \
	_rb_define_module_function(module, #entity "_playing?", audio_##entity##IsPlaying);

#define INIT_AUD_PROP_BIND(PropName, prop_name_s) \
{ \
	_rb_define_module_function(module, prop_name_s, audio##Get##PropName); \
	_rb_define_module_function(module, prop_name_s "=", audio##Set##PropName); \
}

void
audioBindingInit()
{
	VALUE module = rb_define_module("Audio");

	BIND_PLAY_STOP_FADE( bgm );
	BIND_PLAY_STOP_FADE( bgs );
	BIND_PLAY_STOP_FADE( me  );

	BIND_POS( bgm );
	BIND_POS( bgs );

	BIND_PLAY_STOP( se )

	BIND_IS_PLAYING( bgm );
	BIND_IS_PLAYING( bgs );
	BIND_IS_PLAYING( me );

	_rb_define_module_function(module, "__reset__", audioReset);

	INIT_AUD_PROP_BIND( BGM_Volume, "bgm_volume" );
	INIT_AUD_PROP_BIND( SFX_Volume, "sfx_volume" );
}
