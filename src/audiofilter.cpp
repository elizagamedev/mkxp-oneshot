/*
** audiofilter.cpp
**
** This file is part of ModShot.
*/

#include "audiofilter.h"
#include "al-util.h"
#include <stdexcept>

/* Base AudioFilter */

void AudioFilter::process(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq) {
    switch(format){
        case AL_FORMAT_MONO8:
            process_mono((uint8_t*) data, size, freq);
            break;
        case AL_FORMAT_MONO16:
            process_mono((uint16_t*) data, size, freq);
            break;
        case AL_FORMAT_STEREO8:
            process_stereo((uint8_t*) data, size, freq);
            break;
        case AL_FORMAT_STEREO16:
            process_stereo((uint16_t*) data, size, freq);
            break;
    }
}

void AudioFilter::process_mono(uint8_t *data, int size, int freq) {
    throw std::logic_error("Not Implemented");
}

void AudioFilter::process_mono(uint16_t *data, int size, int freq) {
    throw std::logic_error("Not Implemented");
}

void AudioFilter::process_stereo(uint8_t *data, int size, int freq) {
    throw std::logic_error("Not Implemented");
}

void AudioFilter::process_stereo(uint16_t *data, int size, int freq) {
    throw std::logic_error("Not Implemented");
}

/* Rectifier filter */

RectifierAudioFilter::RectifierAudioFilter(float probability){
    threshold = (unsigned int) (RAND_MAX * probability);
    if (threshold < 0)
        threshold = RAND_MAX;
}

void RectifierAudioFilter::process_mono(uint8_t *data, int size, int freq) {
    rectify(data, size / sizeof(uint8_t));
}

void RectifierAudioFilter::process_mono(uint16_t *data, int size, int freq) {
    rectify(data, size / sizeof(uint16_t));
}

void RectifierAudioFilter::process_stereo(uint8_t *data, int size, int freq) {
    rectify(data, size / sizeof(uint8_t));
}

void RectifierAudioFilter::process_stereo(uint16_t *data, int size, int freq) {
    rectify(data, size / sizeof(uint16_t));
}

void RectifierAudioFilter::rectify(uint8_t *data, int count) {
    for (int i = 0; i < count; i++) {
        if (data[i] & 0x80 && rand() < threshold) {
            data[i] = ~data[i];
        }
    }
}

void RectifierAudioFilter::rectify(uint16_t *data, int count) {
    for (int i = 0; i < count; i++) {
        if (data[i] & 0x8000 && rand() < threshold) {
            data[i] = ~data[i];
        }
    }
}
