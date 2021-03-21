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
#include "audiofilter.h"

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

#define DEF_CROSSFADE(entity) \
RB_METHOD(audio_##entity##Crossfade) \
{ \
	RB_UNUSED_PARAM; \
	const char *filename; \
	double time = 2; \
	int volume = 100; \
	int pitch = 100; \
	double pos = -1.0; \
	rb_get_args(argc, argv, "z|fiif", &filename, &time, &volume, &pitch, &pos RB_ARG_END); \
	GUARD_EXC(shState->audio().entity##Crossfade(filename, time, volume, pitch, pos);) \
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

AudioFilter* constructAudioFilter(int argc, VALUE *argv) {
	char* filtertype;
	rb_get_args(argc, argv, "z|", &filtertype RB_ARG_END);
	if (!strcmp(filtertype, "Rectifier")) {
		double intensity;
		rb_get_args(argc, argv, "zf", &filtertype, &intensity RB_ARG_END);
		return new RectifierAudioFilter(intensity);
	}
	return NULL;
}

AL::Filter::ID constructALFilter(int argc, VALUE *argv) {
	int type;
	double gain, gainlf, gainhf;
	rb_get_args(argc, argv, "i|", &type RB_ARG_END);
	switch(type) {
		case 0: // lowpass
			rb_get_args(argc, argv, "iff", &type, &gain, &gainhf RB_ARG_END);
			return AL::Filter::createLowpassFilter(gain, gainhf);
		case 1: // highpass
			rb_get_args(argc, argv, "iff", &type, &gain, &gainlf RB_ARG_END);
			return AL::Filter::createHighpassFilter(gain, gainlf);
		case 2: // bandpass
			rb_get_args(argc, argv, "ifff", &type, &gain, &gainlf, &gainhf RB_ARG_END);
			return AL::Filter::createBandpassFilter(gain, gainlf, gainhf);
		default:
			rb_raise(rb_eArgError, "Unrecognized AL filter type");
	}
}

#define DEF_AUD_ALFILTER(entity) \
	RB_METHOD(audio_##entity##SetALFilter) { \
		AL::Filter::ID filter = constructALFilter(argc, argv); \
		shState->audio().entity##SetALFilter(filter); \
		return Qnil; \
	} \
	RB_METHOD(audio_##entity##ClearALFilter) { \
		shState->audio().entity##ClearALFilter(); \
		return Qnil; \
	} \
	RB_METHOD(audio_##entity##SetALEffect) { \
		VALUE effect_obj; \
		rb_get_args(argc, argv, "o", &effect_obj RB_ARG_END); \
		ALuint effect = NUM2INT(rb_funcall(effect_obj, rb_intern("create_underlying_effect"), 0)); \
		shState->audio().entity##SetALEffect(effect); \
		return Qnil; \
	} \
	RB_METHOD(audio_##entity##ClearALEffect) { \
		shState->audio().entity##ClearALEffect(); \
		return Qnil; \
	} \

#define DEF_AUD_FILTER(entity) \
	RB_METHOD(audio_##entity##AddFilter) { \
		AudioFilter* af = constructAudioFilter(argc, argv); \
		if (af == NULL) { \
			rb_raise(rb_eArgError, "Unrecognized audio filter type"); \
		} \
		shState->audio().entity##AddFilter(af); \
		return Qnil; \
	} \
	\
	RB_METHOD(audio_##entity##ClearFilters) { \
		shState->audio().entity##ClearFilters(); \
		return Qnil; \
	} \
	DEF_AUD_ALFILTER(entity)


DEF_PLAY_STOP_POS( bgm )
DEF_PLAY_STOP_POS( bgs )

DEF_PLAY_STOP( me )

DEF_FADE( bgm )
DEF_FADE( bgs )
DEF_FADE( me )

DEF_CROSSFADE( bgm )
DEF_CROSSFADE( bgs )
DEF_CROSSFADE( me )

DEF_ISPLAYING( bgm )
DEF_ISPLAYING( bgs )
DEF_ISPLAYING( me )

DEF_PLAY_STOP( se )

DEF_AUD_PROP_I(BGM_Volume)
DEF_AUD_PROP_I(SFX_Volume)

DEF_AUD_FILTER(bgm)
DEF_AUD_FILTER(bgs)
DEF_AUD_FILTER(me)
DEF_AUD_ALFILTER(se)

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

#define BIND_CROSSFADE(entity) \
	_rb_define_module_function(module, #entity "_crossfade", audio_##entity##Crossfade);

#define BIND_PLAY_STOP_FADE_CROSS(entity) \
	BIND_PLAY_STOP(entity) \
	BIND_FADE(entity) \
	BIND_CROSSFADE(entity)

#define BIND_POS(entity) \
	_rb_define_module_function(module, #entity "_pos", audio_##entity##Pos);

#define BIND_IS_PLAYING(entity) \
	_rb_define_module_function(module, #entity "_playing?", audio_##entity##IsPlaying);

#define INIT_AUD_PROP_BIND(PropName, prop_name_s) \
{ \
	_rb_define_module_function(module, prop_name_s, audio##Get##PropName); \
	_rb_define_module_function(module, prop_name_s "=", audio##Set##PropName); \
}

#define BIND_AUDIO_ALFILTER(entity) \
	_rb_define_module_function(module, #entity "_set_al_filter", audio_##entity##SetALFilter); \
	_rb_define_module_function(module, #entity "_clear_al_filter", audio_##entity##ClearALFilter); \
	_rb_define_module_function(module, #entity "_set_al_effect", audio_##entity##SetALEffect); \
	_rb_define_module_function(module, #entity "_clear_al_effect", audio_##entity##ClearALEffect); \

#define BIND_AUDIO_FILTER(entity) \
	_rb_define_module_function(module, #entity "_add_filter", audio_##entity##AddFilter); \
	_rb_define_module_function(module, #entity "_clear_filters", audio_##entity##ClearFilters); \
	BIND_AUDIO_ALFILTER(entity)

void
audioBindingInit()
{
	VALUE module = rb_define_module("Audio");

	BIND_PLAY_STOP_FADE_CROSS( bgm );
	BIND_PLAY_STOP_FADE_CROSS( bgs );
	BIND_PLAY_STOP_FADE_CROSS( me  );

	BIND_POS( bgm );
	BIND_POS( bgs );

	BIND_PLAY_STOP( se )

	BIND_IS_PLAYING( bgm );
	BIND_IS_PLAYING( bgs );
	BIND_IS_PLAYING( me );

	BIND_AUDIO_FILTER(bgm);
	BIND_AUDIO_FILTER(bgs);
	BIND_AUDIO_FILTER(me);
	BIND_AUDIO_ALFILTER(se);

	_rb_define_module_function(module, "__reset__", audioReset);

	INIT_AUD_PROP_BIND( BGM_Volume, "bgm_volume" );
	INIT_AUD_PROP_BIND( SFX_Volume, "sfx_volume" );
}
