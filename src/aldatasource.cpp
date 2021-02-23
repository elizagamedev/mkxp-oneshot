/*
** aldatasource.cpp
**
** This file is part of ModShot.
*/

#include "aldatasource.h"

void ALDataSource::addFilter(AudioFilter* filter) {
    SDL_LockMutex(filterMut);
    filters.push_back(filter);
    SDL_UnlockMutex(filterMut);
}

void ALDataSource::clearFilters() {
    SDL_LockMutex(filterMut);
    for(std::list<AudioFilter*>::iterator i = filters.begin(); i != filters.end(); i++) {
        delete *i;
    }
    filters.clear();
    SDL_UnlockMutex(filterMut);
}

void ALDataSource::applyFilters(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq) {
    SDL_LockMutex(filterMut);
    for(std::list<AudioFilter*>::iterator i = filters.begin(); i != filters.end(); i++) {
        (*i)->process(format, data, size, freq);
    }
    SDL_UnlockMutex(filterMut);
}
