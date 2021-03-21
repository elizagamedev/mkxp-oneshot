/*
** audiofilter.cpp
**
** This file is part of ModShot.
*/

#include "audiofilter.h"
#include "al-util.h"
#include <util.h>
#include <stdexcept>

/* Base AudioFilter */

void AudioFilter::process(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq) {
    switch(format){
        case AL_FORMAT_MONO8:
            process_mono((uint8_t*) data, size, freq);
            break;
        case AL_FORMAT_MONO16:
            process_mono((int16_t*) data, size, freq);
            break;
        case AL_FORMAT_STEREO8:
            process_stereo((uint8_t*) data, size, freq);
            break;
        case AL_FORMAT_STEREO16:
            process_stereo((int16_t*) data, size, freq);
            break;
    }
}

void AudioFilter::process_mono(uint8_t *data, int size, int freq) {
    throw std::logic_error("AudioFilter cannot process 8bit mono data");
}

void AudioFilter::process_mono(int16_t *data, int size, int freq) {
    throw std::logic_error("AudioFilter cannot process 16bit mono data");
}

void AudioFilter::process_stereo(uint8_t *data, int size, int freq) {
    throw std::logic_error("AudioFilter cannot process 8bit stereo data");

}

void AudioFilter::process_stereo(int16_t *data, int size, int freq) {
    throw std::logic_error("AudioFilter cannot process 16bit stereo data");

}

/* Rectifier filter */

RectifierAudioFilter::RectifierAudioFilter(float intensity) {
    this->intensity = clamp(intensity, 0.0f, 1.0f);
}

void RectifierAudioFilter::process_mono(uint8_t *data, int size, int freq) {
    rectify(data, size / sizeof(uint8_t));
}

void RectifierAudioFilter::process_mono(int16_t *data, int size, int freq) {
    rectify(data, size / sizeof(int16_t));
}

void RectifierAudioFilter::process_stereo(uint8_t *data, int size, int freq) {
    rectify(data, size / sizeof(uint8_t));
}

void RectifierAudioFilter::process_stereo(int16_t *data, int size, int freq) {
    rectify(data, size / sizeof(int16_t));
}

void RectifierAudioFilter::rectify(uint8_t *data, int count) {
    for (int i = 0; i < count; i++) {
        if (!(data[i] & 0x80)) {
            data[i] = ((uint8_t) clamp((int) (-(int8_t) (data[i] ^ 0x80) * 2 * intensity) + (int8_t) (data[i] ^ 0x80), -0x80, 0x7f)) ^ 0x80;
        }
    }
}

void RectifierAudioFilter::rectify(int16_t *data, int count) {
    for (int i = 0; i < count; i++) {
        if (data[i] < 0) {
            data[i] = (int16_t) clamp((int) ((-(int)data[i]) * 2 * intensity) + data[i], -0x8000, 0x7fff);
        }
    }
}
