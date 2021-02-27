/*
** audio.cpp
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

#include "audiostream.h"
#include "soundemitter.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "sdl-util.h"

#include <string>

#include <SDL_thread.h>
#include <SDL_timer.h>

struct AudioPrivate
{
	int bgm_volume;
	int sfx_volume;

	AudioStream bgm;
	AudioStream bgs;
	AudioStream me;

	int current_bgm_volume;
	int current_bgs_volume;
	int current_me_volume;

	SoundEmitter se;

	SyncPoint &syncPoint;

	/* The 'MeWatch' is responsible for detecting
	 * a playing ME, quickly fading out the BGM and
	 * keeping it paused/stopped while the ME plays,
	 * and unpausing/fading the BGM back in again
	 * afterwards */
	enum MeWatchState
	{
		MeNotPlaying,
		BgmFadingOut,
		MePlaying,
		BgmFadingIn
	};

	struct
	{
		SDL_Thread *thread;
		AtomicFlag termReq;
		MeWatchState state;
	} meWatch;

	AudioPrivate(RGSSThreadData &rtData)
	    : bgm(ALStream::Looped, "bgm"),
	      bgs(ALStream::Looped, "bgs"),
	      me(ALStream::NotLooped, "me"),
	      se(rtData.config),
	      syncPoint(rtData.syncPoint)
	{
		bgm_volume = 100;
		sfx_volume = 100;
		current_bgm_volume = 100;
		current_bgs_volume = 100;
		current_me_volume = 100;
		meWatch.state = MeNotPlaying;
		meWatch.thread = createSDLThread
			<AudioPrivate, &AudioPrivate::meWatchFun>(this, "audio_mewatch");
	}

	~AudioPrivate()
	{
		meWatch.termReq.set();
		SDL_WaitThread(meWatch.thread, 0);
	}

	void meWatchFun()
	{
		const float fadeOutStep = 1.f / (200  / AUDIO_SLEEP);
		const float fadeInStep  = 1.f / (1000 / AUDIO_SLEEP);

		while (true)
		{
			syncPoint.passSecondarySync();

			if (meWatch.termReq)
				return;

			switch (meWatch.state)
			{
			case MeNotPlaying:
			{
				me.lockStream();

				if (me.queryState() == ALStream::Playing)
				{
					/* ME playing detected. -> FadeOutBGM */
					bgm.extPaused = true;
					meWatch.state = BgmFadingOut;
				}

				me.unlockStream();

				break;
			}

			case BgmFadingOut :
			{
				me.lockStream();

				if (me.queryState() != ALStream::Playing)
				{
					/* ME has ended while fading OUT BGM. -> FadeInBGM */
					me.unlockStream();
					meWatch.state = BgmFadingIn;

					break;
				}

				bgm.lockStream();

				float vol = bgm.getVolume(AudioStream::External);
				vol -= fadeOutStep;

				if (vol < 0 || bgm.queryState() != ALStream::Playing)
				{
					/* Either BGM has fully faded out, or stopped midway. -> MePlaying */
					bgm.setVolume(AudioStream::External, 0);
					bgm.pause();
					meWatch.state = MePlaying;
					bgm.unlockStream();
					me.unlockStream();

					break;
				}

				bgm.setVolume(AudioStream::External, vol);
				bgm.unlockStream();
				me.unlockStream();

				break;
			}

			case MePlaying :
			{
				me.lockStream();

				if (me.queryState() != ALStream::Playing)
				{
					/* ME has ended */
					bgm.lockStream();

					bgm.extPaused = false;

					ALStream::State sState = bgm.queryState();

					if (sState == ALStream::Paused)
					{
						/* BGM is paused. -> FadeInBGM */
						bgm.streams[0].play();
						meWatch.state = BgmFadingIn;
					}
					else
					{
						/* BGM is stopped. -> MeNotPlaying */
						bgm.setVolume(AudioStream::External, 1.0f);

						if (!bgm.noResumeStop)
							bgm.streams[0].play();

						meWatch.state = MeNotPlaying;
					}

					bgm.unlockStream();
				}

				me.unlockStream();

				break;
			}

			case BgmFadingIn :
			{
				bgm.lockStream();

				if (bgm.queryState() == ALStream::Stopped)
				{
					/* BGM stopped midway fade in. -> MeNotPlaying */
					bgm.setVolume(AudioStream::External, 1.0f);
					meWatch.state = MeNotPlaying;
					bgm.unlockStream();

					break;
				}

				me.lockStream();

				if (me.queryState() == ALStream::Playing)
				{
					/* ME started playing midway BGM fade in. -> FadeOutBGM */
					bgm.extPaused = true;
					meWatch.state = BgmFadingOut;
					me.unlockStream();
					bgm.unlockStream();

					break;
				}

				float vol = bgm.getVolume(AudioStream::External);
				vol += fadeInStep;

				if (vol >= 1)
				{
					/* BGM fully faded in. -> MeNotPlaying */
					vol = 1.0f;
					meWatch.state = MeNotPlaying;
				}

				bgm.setVolume(AudioStream::External, vol);

				me.unlockStream();
				bgm.unlockStream();

				break;
			}
			}

			SDL_Delay(AUDIO_SLEEP);
		}
	}
};

Audio::Audio(RGSSThreadData &rtData)
	: p(new AudioPrivate(rtData))
{}


void Audio::bgmPlay(const char *filename,
                    int volume,
                    int pitch,
                    float pos,
					bool fadeInOnOffset)
{
	p->current_bgm_volume = volume;
	p->bgm.play(filename, (volume*p->bgm_volume)/100, pitch, pos, fadeInOnOffset);
}

void Audio::bgmStop()
{
	p->bgm.stop();
}

void Audio::bgmFade(int time)
{
	p->bgm.fadeOut(time);
}


void Audio::bgsPlay(const char *filename,
                    int volume,
                    int pitch,
                    float pos,
					bool fadeInOnOffset)
{
	p->current_bgs_volume = volume;
	p->bgs.play(filename, (volume*p->sfx_volume)/100, pitch, pos, fadeInOnOffset);
}

void Audio::bgsStop()
{
	p->bgs.stop();
}

void Audio::bgsFade(int time)
{
	p->bgs.fadeOut(time);
}


void Audio::mePlay(const char *filename,
                   int volume,
                   int pitch)
{
	p->current_me_volume = volume;
	p->me.play(filename, (volume*p->bgm_volume)/100, pitch);
}

void Audio::meStop()
{
	p->me.stop();
}

void Audio::meFade(int time)
{
	p->me.fadeOut(time);
}


void Audio::sePlay(const char *filename,
                   int volume,
                   int pitch)
{
	p->se.play(filename, (volume*p->sfx_volume)/100, pitch);
}

void Audio::seStop()
{
	p->se.stop();
}

void Audio::bgmCrossfade(const char *filename,
						 float time,
			       		 int volume,
				   		 int pitch,
				   		 float offset)
{
	p->bgm.crossfade(filename, time, volume, pitch, offset);
}

void Audio::bgsCrossfade(const char *filename,
						 float time,
			       		 int volume,
				   		 int pitch,
				   		 float offset)
{
	p->bgs.crossfade(filename, time, volume, pitch, offset);
}

void Audio::meCrossfade(const char *filename,
						float time,
			       		int volume,
				   		int pitch,
				   		float offset)
{
	p->me.crossfade(filename, time, volume, pitch, offset);
}

float Audio::bgmPos()
{
	return p->bgm.playingOffset();
}

float Audio::bgsPos()
{
	return p->bgs.playingOffset();
}

bool Audio::bgmIsPlaying()
{
	return p->bgm.queryState();
}

bool Audio::bgsIsPlaying()
{
	return p->bgs.queryState();
}

bool Audio::meIsPlaying()
{
	return p->me.queryState();
}

void Audio::reset()
{
	p->bgm.stop();
	p->bgs.stop();
	p->me.stop();
	p->se.stop();
}

int Audio::getBGM_Volume() const
{
	return p->bgm_volume;
}

void Audio::setBGM_Volume(int value)
{
	if(value > 100){
		value = 100;
	}else if(value < 0){
		value = 0;
	}
	p->bgm_volume = value;
	p->bgm.lockStream();
	p->bgm.setVolume(AudioStream::Base, ((float)(p->bgm_volume * p->current_bgm_volume))/10000.0f );
	p->bgm.unlockStream();
	p->me.lockStream();
	p->me.setVolume(AudioStream::Base, ((float)(p->bgm_volume * p->current_me_volume))/10000.0f );
	p->me.unlockStream();
}

int Audio::getSFX_Volume() const
{
	return p->sfx_volume;
}

void Audio::setSFX_Volume(int value)
{
	if(value > 100){
		value = 100;
	}else if(value < 0){
		value = 0;
	}
	p->sfx_volume = value;
	p->bgs.lockStream();
	p->bgs.setVolume(AudioStream::Base, ((float)(p->sfx_volume * p->current_bgs_volume))/10000.0f );
	p->bgs.unlockStream();
	
}

#define AUDIO_CPP_DEF_ALFILTER_FUNCS(entity) \
	void Audio::entity##SetALFilter(AL::Filter::ID filter) { \
		p->entity.setALFilter(filter); \
	} \
	\
	void Audio::entity##ClearALFilter() { \
		p->entity.clearALFilter(); \
	} \
	\
	void Audio::entity##SetALEffect(ALuint effect) { \
		p->entity.setALEffect(effect); \
	} \
	\
	void Audio::entity##ClearALEffect() { \
		p->entity.clearALEffect(); \
	}

#define AUDIO_CPP_DEF_FILTER_FUNCS(entity) \
	void Audio::entity##AddFilter(AudioFilter* filter) { \
		p->entity.addFilter(filter); \
	} \
	\
	void Audio::entity##ClearFilters() { \
		p->entity.clearFilters(); \
	} \
	\
	AUDIO_CPP_DEF_ALFILTER_FUNCS(entity)

AUDIO_CPP_DEF_FILTER_FUNCS(bgm)
AUDIO_CPP_DEF_FILTER_FUNCS(bgs)
AUDIO_CPP_DEF_FILTER_FUNCS(me)
AUDIO_CPP_DEF_ALFILTER_FUNCS(se)

Audio::~Audio() { delete p; }
