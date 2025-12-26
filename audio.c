#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include "audio.h"

bool audio_init(Audio *audio)
{
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_Log("Failed to initialize SDL audio: %s", SDL_GetError());
        return false;
    }

    audio->sample_rate = SAMPLE_RATE;

    SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = 1,
        .freq = audio->sample_rate
    };

    audio->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!audio->stream) {
        SDL_Log("Failed to open audio device: %s", SDL_GetError());
        return false;
    }

    SDL_ResumeAudioStreamDevice(audio->stream);
    return true;
}

void audio_update(Audio *audio, uint8_t sound_timer)
{
    if (!audio->stream) return;

    if (sound_timer > 0) {
        /* Generate beep for this frame (~16.67ms at 60Hz) */
        int sample_count = audio->sample_rate / 60; /* 60Hz timer updates */
        float *samples = malloc(sample_count * sizeof(float));

        float frequency = 440.0f; /* A note */
        for (int i = 0; i < sample_count; i++) {
            float t = (float)i / audio->sample_rate;
            samples[i] = 0.3f * sinf(2.0f * M_PI * frequency * t);
        }

        SDL_PutAudioStreamData(audio->stream, samples, sample_count * sizeof(float));
        free(samples);
    }
}

void audio_cleanup(Audio *audio)
{
    if (audio->stream) {
        SDL_DestroyAudioStream(audio->stream);
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}