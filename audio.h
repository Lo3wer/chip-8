#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define SAMPLE_RATE 44100

typedef struct {
    SDL_AudioStream *stream;
    int sample_rate;
} Audio;

bool audio_init(Audio *audio);
void audio_update(Audio *audio, uint8_t sound_timer);
void audio_cleanup(Audio *audio);

#endif