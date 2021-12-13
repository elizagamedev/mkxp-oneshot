/*
** audio.h
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

#ifndef AUDIO_H
#define AUDIO_H

#include "util.h"
#include "al-util.h"

/* Concerning the 'pos' parameter:
 *   RGSS3 actually doesn't specify a format for this,
 *   it's only implied that it is a numerical value
 *   (must be 0 on invalid cases), and it's not used for
 *   anything outside passing it back into bgm/bgs_play.
 *   We use this freedom to define pos to be a float,
 *   in seconds of playback. (RGSS3 seems to use large
 *   integers that _look_ like sample offsets but I can't
 *   quite make out their meaning yet) */

struct AudioPrivate;
struct RGSSThreadData;

class Audio
{
public:
	void bgmPlay(const char *filename,
	             int volume = 100,
	             int pitch = 100,
	             float pos = -1,
				 bool fadeInOnOffset = true);
	void bgmStop();
	void bgmFade(int time);

	void bgsPlay(const char *filename,
	             int volume = 100,
	             int pitch = 100,
	             float pos = -1,
				 bool fadeInOnOffset = true);
	void bgsStop();
	void bgsFade(int time);

	void mePlay(const char *filename,
	            int volume = 100,
	            int pitch = 100);
	void meStop();
	void meFade(int time);

	void sePlay(const char *filename,
	            int volume = 100,
	            int pitch = 100);
	void seStop();

	void lchPlay(unsigned int id,
				 const char *filename,
	             int volume = 100,
	             int pitch = 100,
	             float pos = -1,
				 bool fadeInOnOffset = true);
	void lchStop(unsigned int id);
	void lchFade(unsigned int id, int time);

	void chPlay(unsigned int id,
				 const char *filename,
	             int volume = 100,
	             int pitch = 100,
	             float pos = -1,
				 bool fadeInOnOffset = true);
	void chStop(unsigned int id);
	void chFade(unsigned int id, int time);

	void bgmCrossfade(const char *filename,
				 	  float time = 2,
			     	  int volume = 100,
				 	  int pitch = 100,
					  float offset = -1);
	void bgsCrossfade(const char *filename,
				 	  float time = 2,
			     	  int volume = 100,
				 	  int pitch = 100,
					  float offset = -1);
	void meCrossfade(const char *filename,
					 float time = 2,
			    	 int volume = 100,
					 int pitch = 100,
					 float offset = -1);
	void lchCrossfade(unsigned int id,
					  const char *filename,
					  float time = 2,
			    	  int volume = 100,
					  int pitch = 100,
					  float offset = -1);
	void chCrossfade(unsigned int id,
					  const char *filename,
					  float time = 2,
			    	  int volume = 100,
					  int pitch = 100,
					  float offset = -1);

	float bgmPos();
	float bgsPos();
	float lchPos(unsigned int id);
	float chPos(unsigned int id);

	bool bgmIsPlaying();
	bool bgsIsPlaying();
	bool meIsPlaying();
	bool lchIsPlaying(unsigned int id);
	bool chIsPlaying(unsigned int id);

#define AUDIO_H_DECL_ALFILTER_FUNCS(entity) \
	void entity##SetALFilter(AL::Filter::ID filter); \
	void entity##ClearALFilter(); \
	void entity##SetALEffect(ALuint effect); \
	void entity##ClearALEffect();

	AUDIO_H_DECL_ALFILTER_FUNCS(bgm)
	AUDIO_H_DECL_ALFILTER_FUNCS(bgs)
	AUDIO_H_DECL_ALFILTER_FUNCS(me)
	AUDIO_H_DECL_ALFILTER_FUNCS(se)

#define AUDIO_H_DECL_CH_ALFILER_FUNCS(entity) \
	void entity##SetALFilter(unsigned int id, AL::Filter::ID filter); \
	void entity##ClearALFilter(unsigned int id); \
	void entity##SetALEffect(unsigned int id, ALuint effect); \
	void entity##ClearALEffect(unsigned int id);

	AUDIO_H_DECL_CH_ALFILER_FUNCS(lch)
	AUDIO_H_DECL_CH_ALFILER_FUNCS(ch)

	void reset();

    /* Non-standard extension */
	DECL_ATTR(BGM_Volume, int)
    DECL_ATTR(SFX_Volume, int)

#define AUDIO_H_DECL_CH_SPECIAL_FUNCS(entity) \
	float get##entity##Volume(unsigned int id); \
	void set##entity##Volume(unsigned int id, float volume); \
	float get##entity##GlobalVolume(); \
	void set##entity##GlobalVolume(float volume); \
	float get##entity##Pitch(unsigned int id); \
	void set##entity##Pitch(unsigned int id, float pitch); \
	unsigned int entity##Size(); \
	void entity##Resize(unsigned int size);

	AUDIO_H_DECL_CH_SPECIAL_FUNCS(lch)
	AUDIO_H_DECL_CH_SPECIAL_FUNCS(ch)

private:
	Audio(RGSSThreadData &rtData);
	~Audio();

	friend struct SharedStatePrivate;

	AudioPrivate *p;
};

#endif // AUDIO_H
