/*
** audiostream.h
**
** This file is part of ModShot.
*/

#ifndef AUDIOFILTER_H
#define AUDIOFILTER_H

#include "al-util.h"

class AudioFilter {
    public:
    void process(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);

    protected:
    virtual void process_mono(uint8_t *data, int size, int freq);
    virtual void process_mono(uint16_t *data, int size, int freq);
    virtual void process_stereo(uint8_t *data, int size, int freq);
    virtual void process_stereo(uint16_t *data, int size, int freq);
};

class RectifierAudioFilter : public AudioFilter {
    public:
    RectifierAudioFilter(float probability = 0.5);
    unsigned int threshold;

    private:
    void process_mono(uint8_t *data, int size, int freq);
    void process_mono(uint16_t *data, int size, int freq);
    void process_stereo(uint8_t *data, int size, int freq);
    void process_stereo(uint16_t *data, int size, int freq);
    void rectify(uint8_t *data, int count);
    void rectify(uint16_t *data, int count);
};

#endif // AUDIOFILTER_H
