#include <math.h>
#include <SDL2/SDL.h>
#include "sound.h"

void audio_callback(void *data, unsigned char *buffer, int bytes)
{
    double sample_rate = 44100.0;
    int amplitude = 28000;

    Sint16* soundbuffer = (Sint16*) buffer;
    int len = bytes / 2;
    int sample = *(int*) data;

    for (int i = 0; i < len; i++, sample++)
    {
        double time = sample / sample_rate;
        soundbuffer[i] = (Sint16)(amplitude * sin(2.0f * M_PI * 441.0f * time));
    }
}

void make_sound()
{
    int sample = 0;
    SDL_AudioSpec want;
    SDL_zero(want);

    want.freq = 44100;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 2048;
    want.callback = audio_callback;
    want.userdata = &sample;

    SDL_AudioSpec have;
    if (SDL_OpenAudio(&want, &have) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
    }
    if (want.format != have.format)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get AudioSpecs");
    }

    SDL_PauseAudio(0);
    SDL_Delay(25);
    SDL_PauseAudio(1);
}

void init_audio()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    }
}

void stop_audio()
{
    SDL_CloseAudio();
}