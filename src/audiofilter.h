/*
** audiostream.h
**
** This file is part of ModShot.
*/

#ifndef AUDIOFILTER_H
#define AUDIOFILTER_H

#include "al-util.h"
#include <queue>

class AudioFilter {
    public:
    void process(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);

    protected:
    // note to anyone implementing a filter:
    // 8 bit data (uint8_t) is unsigned. the midpoint would be 128, low point is 0 and high point is 255.
    // (to convert it into 8 bit signed, you can ^=0x80 (flip MSB))
    // 16 bit data (int16_t) is signed. the midpoint is 0, low point is 0x8000 (-32768) and high point is 0x7fff (+32767)
    virtual void process_mono(uint8_t *data, int size, int freq);
    virtual void process_mono(int16_t *data, int size, int freq);
    virtual void process_stereo(uint8_t *data, int size, int freq);
    virtual void process_stereo(int16_t *data, int size, int freq);
};

class RectifierAudioFilter : public AudioFilter {
    public:
    RectifierAudioFilter(float intensity = 0.5);
    float intensity;

    private:
    void process_mono(uint8_t *data, int size, int freq);
    void process_mono(int16_t *data, int size, int freq);
    void process_stereo(uint8_t *data, int size, int freq);
    void process_stereo(int16_t *data, int size, int freq);
    void rectify(uint8_t *data, int count);
    void rectify(int16_t *data, int count);
};

#endif // AUDIOFILTER_H
