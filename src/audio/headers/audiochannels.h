/*
** audiochannels.h
**
** This file is part of ModShot.
*/

#ifndef AUDIOCHANNELS_H
#define AUDIOCHANNELS_H

#include "audiostream.h"
#include <vector>

class AudioChannels {
    public:
    AudioChannels(ALStream::LoopMode loopMode,
	            const std::string &threadId,
                unsigned int count);

    unsigned int size();
    void resize(unsigned int size);
    float getGlobalVolume();
    void setGlobalVolume(float volume);

    void play(unsigned int id,
              const std::string filename,
	          int volume,
	          int pitch,
	          float offset = 0,
			  bool fadeInOnOffset = true);
    void crossfade(unsigned int id,
                   const std::string &filename,
				   float time,
			       int volume,
				   int pitch,
				   float offset = 0);
	void pause(unsigned int id);
	void stop(unsigned int id);
    void stopall();
	void fadeOut(unsigned int id, int duration);
	void setVolume(unsigned int id, AudioStream::VolumeType type, float value);
	float getVolume(unsigned int id, AudioStream::VolumeType type);
	void setPitch(unsigned int id, float value);
	float getPitch(unsigned int id);
	float playingOffset(unsigned int id);
	ALStream::State queryState(unsigned int id);
	void setALFilter(unsigned int id, AL::Filter::ID filter);
	void setALEffect(unsigned int id, ALuint effect);

    private:
    std::vector<AudioStream*> streams;
    ALStream::LoopMode loopMode;
    const std::string threadId;
    float globalVolume;
};

#endif // AUDIOCHANNELS_H
