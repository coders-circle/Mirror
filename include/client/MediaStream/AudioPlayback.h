#pragma once

#include "MediaStream.h"
#include <SDL2/SDL.h>

class AudioPlayback
{
public:
    AudioPlayback() : m_decoder(NULL), m_decoderContext(NULL), m_wantedSpec(NULL),
                       m_obtainedSpec(NULL)
                       {}
    void Initialize();
    void DecodeFrame(); 
    void StartPlayback();

private:
    AVCodecID m_codecID;    //codec id
    AVCodec *m_decoder;     // decoder
    AVCodecContext *m_decoderContext;   // Decoder Context
    SDL_AudioSpec *m_wantedSpec, *m_obtainedSpec;
};


static void AudioCallback(void *userdata, uint8_t *stream, int len)
{
}
