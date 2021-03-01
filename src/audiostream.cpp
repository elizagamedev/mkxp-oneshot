/*
** audiostream.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2014 Jonas Kulla <Nyocurio@gmail.com>
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

#include "audiostream.h"

#include "util.h"
#include "exception.h"

#include <SDL_mutex.h>
#include <SDL_thread.h>
#include <SDL_timer.h>

AudioStream::AudioStream(ALStream::LoopMode loopMode,
                         const std::string &threadId)
	: extPaused(false),
	  noResumeStop(false),
	  alStreamThreadID(0)
{
	current.volume = 1.0f;
	current.pitch = 1.0f;

	for (size_t i = 0; i < VolumeTypeCount; ++i)
		volumes[i] = 1.0f;

	fade.thread = 0;
	fade.threadName = std::string("audio_fadeout (") + threadId + ")";

	fadeIn.thread = 0;
	fadeIn.threadName = std::string("audio_fadein (") + threadId + ")";

	effectSlot = AL::AuxiliaryEffectSlot::gen();

	alStreamThreadIDPrefix = threadId + "_";
	streams.emplace_front(loopMode, effectSlot, alStreamThreadIDPrefix + std::to_string(alStreamThreadID));
	alStreamThreadID++;


	streamMut = SDL_CreateMutex();

	crossfademgr.threadName = std::string("audio_crossfade_mgr (") + threadId + ")";
	crossfademgr.thread = createSDLThread
		<AudioStream, &AudioStream::crossfadeThread>(this, crossfademgr.threadName);
}

AudioStream::~AudioStream()
{
	if (fade.thread)
	{
		fade.reqTerm.set();
		SDL_WaitThread(fade.thread, 0);
	}

	if (fadeIn.thread)
	{
		fadeIn.rqTerm.set();
		SDL_WaitThread(fadeIn.thread, 0);
	}

	crossfademgr.reqTerm.set();
	SDL_WaitThread(crossfademgr.thread, 0);

	lockStream();

	for(ALStream& stream : streams) {
		stream.stop();
		stream.close();
	}

	// no need to reset the list, we're destroying the entire audiostream after all

	unlockStream();

	SDL_DestroyMutex(streamMut);
}

void AudioStream::destroyCrossfades() {
	// if we have crossfades, destroy them all
	while (streams.size() > 1) {
		ALStream& stream = streams.back();
		stream.stop();
		stream.close();
		streams.pop_back();
	}
}

void AudioStream::play(const std::string &filename,
                       int volume,
                       int pitch,
                       float offset,
					   bool fadeInOnOffset)
{
	finiFadeOutInt();

	lockStream();

	float _volume = clamp<int>(volume, 0, 100) / 100.0f;
	float _pitch  = clamp<int>(pitch, 50, 150) / 100.0f;

	ALStream::State sState = streams[0].queryState();

	/* If all parameters match the current ones and we're
	 * still playing, there's nothing to do */
	if (filename == current.filename
	&&  _volume  == current.volume
	&&  _pitch   == current.pitch
	&& offset < 0
	&&  (sState == ALStream::Playing || sState == ALStream::Paused))
	{
		unlockStream();
		return;
	}

	/* If the filenames are equal, and we aren't changing the offset
	 * we update the volume and pitch and continue streaming */
	if (filename == current.filename && offset < 0
	&&  (sState == ALStream::Playing || sState == ALStream::Paused))
	{
		setVolume(Base, _volume);
		streams[0].setPitch(_pitch);
		current.volume = _volume;
		current.pitch = _pitch;
		unlockStream();
		return;
	}

	destroyCrossfades();

	/* Requested audio file is different from current one */
	bool diffFile = (filename != current.filename);

	switch (sState)
	{
	case ALStream::Paused :
	case ALStream::Playing :
		streams[0].stop();
		/* falls through */
	case ALStream::Stopped :
		if (diffFile)
			streams[0].close();
		/* falls through */
	case ALStream::Closed :
		if (diffFile)
		{
			try
			{
				/* This will throw on errors while
				 * opening the data source */
				streams[0].open(filename);
			}
			catch (const Exception &e)
			{
				unlockStream();
				throw e;
			}
		}

		break;
	}

	setVolume(Base, _volume);
	streams[0].setPitch(_pitch);

	if (offset > 0 && fadeInOnOffset == true)
	{
		setVolume(FadeIn, 0);
		startFadeIn();
	}

	current.filename = filename;
	current.volume = _volume;
	current.pitch = _pitch;

	if (!extPaused)
		streams[0].play(offset);
	else
		noResumeStop = false;

	unlockStream();
}

void AudioStream::crossfade(const std::string &filename,
							float time,
			       			int volume,
				   			int pitch,
				   			float offset)
{
	finiFadeOutInt();
	lockStream();

	float _volume = clamp<int>(volume, 0, 100) / 100.0f;
	float _pitch  = clamp<int>(pitch, 50, 150) / 100.0f;
	time = time <= 0 ? 1 : time;
	float fadespeed = 1.0f / (time * 1000 / AUDIO_SLEEP);

	ALStream::State sState = streams[0].queryState();

	if (sState != ALStream::Playing) {
		// just fade in current music, destroy crossfades
		play(filename, volume, pitch, offset, false);
		// but still use our crossfader for the fade in part, because the time is not hardcoded
		streams[0].crossfadeVolume = 0;
		streams[0].crossfadeSpeed = fadespeed;
		unlockStream();
		return;
	}

	// construct new stream for crossfading

	streams.emplace_front(
		streams[0].looped ? ALStream::LoopMode::Looped : ALStream::LoopMode::NotLooped,
		effectSlot,
		alStreamThreadIDPrefix + std::to_string(alStreamThreadID));
	alStreamThreadID++;

	try {
		streams[0].open(filename);
	}
	catch (const Exception &e) {
		// crap, bail ship
		// (and actually destroy new stream, keep old stream)
		streams.erase(streams.begin());
		unlockStream();
		throw e;
	}

	// set first stream to fade in
	streams[0].crossfadeVolume = 0;
	streams[0].crossfadeSpeed = fadespeed;
	// set second stream to fade out
	streams[1].crossfadeSpeed = fadespeed;
	// ignore further streams

	setVolume(Base, _volume);
	streams[0].setPitch(_pitch);

	current.filename = filename;
	current.volume = _volume;
	current.pitch = _pitch;

	if (!extPaused)
		streams[0].play(offset);
	else
		noResumeStop = false;

	unlockStream();
}

void AudioStream::pause()
{
	lockStream();
	streams[0].pause();
	destroyCrossfades();
	unlockStream();
}

void AudioStream::stop()
{
	finiFadeOutInt();

	lockStream();

	noResumeStop = true;

	destroyCrossfades();

	streams[0].stop();

	unlockStream();
}

void AudioStream::fadeOut(int duration)
{
	lockStream();

	ALStream::State sState = streams[0].queryState();
	noResumeStop = true;

	if (fade.active)
	{
		unlockStream();

		return;
	}

	if (sState == ALStream::Paused)
	{
		streams[0].stop();
		unlockStream();

		return;
	}

	if (sState != ALStream::Playing)
	{
		unlockStream();

		return;
	}

	if (fade.thread)
	{
		fade.reqFini.set();
		SDL_WaitThread(fade.thread, 0);
		fade.thread = 0;
	}

	fade.active.set();
	fade.msStep = 1.0f / duration;
	fade.reqFini.clear();
	fade.reqTerm.clear();
	fade.startTicks = SDL_GetTicks();

	fade.thread = createSDLThread
		<AudioStream, &AudioStream::fadeOutThread>(this, fade.threadName);

	unlockStream();
}

/* Any access to this classes 'stream' member,
 * whether state query or modification, must be
 * protected by a 'lock'/'unlock' pair */
void AudioStream::lockStream()
{
	SDL_LockMutex(streamMut);
}

void AudioStream::unlockStream()
{
	SDL_UnlockMutex(streamMut);
}

void AudioStream::setVolume(VolumeType type, float value)
{
	volumes[type] = value;
	updateVolume();
}

float AudioStream::getVolume(VolumeType type)
{
	return volumes[type];
}

float AudioStream::playingOffset()
{
	return streams[0].queryOffset();
}

ALStream::State AudioStream::queryState()
{
	lockStream();
	ALStream::State result = streams[0].queryState();
	unlockStream();
	return result;
}

void AudioStream::addFilter(AudioFilter* filter) {
	lockStream();
	streams[0].addFilter(filter);
	unlockStream();
}

void AudioStream::clearFilters() {
	lockStream();
	streams[0].clearFilters();
	unlockStream();
}

void AudioStream::setALFilter(AL::Filter::ID filter) {
	lockStream();
	for(std::deque<ALStream>::iterator i = streams.begin(); i!=streams.end(); i++) {
		i->setALFilter(filter);
	}
	if(!(curfilter == filter) && !AL::Filter::isNullFilter(curfilter)) {
		AL::Filter::del(curfilter);
	}
	curfilter = filter;
	unlockStream();
}

void AudioStream::setALEffect(ALuint effect) {
	lockStream();
	AL::AuxiliaryEffectSlot::attachEffect(effectSlot, effect);
	if(cureffect != effect && cureffect != AL_EFFECT_NULL) {
		alDeleteEffects(1, &cureffect);
	}
	cureffect = effect;
	unlockStream();
}

void AudioStream::updateVolume()
{
	float vol = GLOBAL_VOLUME;

	for (size_t i = 0; i < VolumeTypeCount; ++i)
		vol *= volumes[i];
	for(ALStream& stream : streams)
		stream.setVolume(vol);
}

void AudioStream::finiFadeOutInt()
{
	if (fade.thread)
	{
		fade.reqFini.set();
		SDL_WaitThread(fade.thread, 0);
		fade.thread = 0;
	}

	if (fadeIn.thread)
	{
		fadeIn.rqFini.set();
		SDL_WaitThread(fadeIn.thread, 0);
		fadeIn.thread = 0;
	}
}

void AudioStream::startFadeIn()
{
	/* Previous fadein should always be terminated in play() */
	assert(!fadeIn.thread);

	fadeIn.rqFini.clear();
	fadeIn.rqTerm.clear();
	fadeIn.startTicks = SDL_GetTicks();

	fadeIn.thread = createSDLThread
		<AudioStream, &AudioStream::fadeInThread>(this, fadeIn.threadName);
}

void AudioStream::fadeOutThread()
{
	while (true)
	{
		/* Just immediately terminate on request */
		if (fade.reqTerm)
			break;

		lockStream();

		uint32_t curDur = SDL_GetTicks() - fade.startTicks;
		float resVol = 1.0f - (curDur*fade.msStep);

		ALStream::State state = streams[0].queryState();

		if (state != ALStream::Playing
		|| resVol < 0
		|| fade.reqFini)
		{
			if (state != ALStream::Paused) {
				streams[0].stop();
				destroyCrossfades();
			}

			setVolume(FadeOut, 1.0f);
			unlockStream();

			break;
		}

		setVolume(FadeOut, resVol);

		unlockStream();

		SDL_Delay(AUDIO_SLEEP);
	}

	fade.active.clear();
}

void AudioStream::fadeInThread()
{
	while (true)
	{
		if (fadeIn.rqTerm)
			break;

		lockStream();

		/* Fade in duration is always 1 second */
		uint32_t cur = SDL_GetTicks() - fadeIn.startTicks;
		float prog = cur / 1000.0f;

		ALStream::State state = streams[0].queryState();

		if (state != ALStream::Playing
		||  prog >= 1.0f
		||  fadeIn.rqFini)
		{
			setVolume(FadeIn, 1.0f);
			unlockStream();

			break;
		}

		/* Quadratic increase (not really the same as
		 * in RMVXA, but close enough) */
		setVolume(FadeIn, prog*prog);

		unlockStream();

		SDL_Delay(AUDIO_SLEEP);
	}
}

void AudioStream::crossfadeThread()
{
	while (true) {
		if (crossfademgr.reqTerm)
			break;
		lockStream();
		if (streams.size() > 1 || streams[0].crossfadeVolume != 1) {
			// fade out streams 1~n
			for (std::deque<ALStream>::iterator i=streams.begin() + 1; i!=streams.end();) {
				i->crossfadeVolume -= i->crossfadeSpeed;
				if (i->crossfadeVolume<0) {
					i->stop();
					i->close();
					i = streams.erase(i);
					continue;
				} else {
					i++;
				}
			}
			// fade in stream 0
			if(streams[0].crossfadeVolume < 1) {
				streams[0].crossfadeVolume += streams[0].crossfadeSpeed;
				if (streams[0].crossfadeVolume >= 1) {
					streams[0].crossfadeVolume = 1;
				}
			}
			updateVolume();
		}
		unlockStream();
		SDL_Delay(AUDIO_SLEEP);
	}
}
