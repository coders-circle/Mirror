#include <common/common.h>
#include <client/MediaStream/AudioPlayback.h>

void AudioPlayback::Initialize()
{
    av_register_all();
    m_codecID = AV_CODEC_ID_AAC;
    m_decoder = avcodec_find_decoder(m_codecID);

    if (!m_decoder)
    {
        throw CodecNotFound();
    }

    m_decoderContext = avcodec_alloc_context3(m_decoder);
    if (!m_decoderContext)
    {
        throw Exception("can't allocate the decoder context");
    }

    avcodec_open2(m_decoderContext, m_decoder, NULL);

    if(!m_wantedSpec)
    {
       m_wantedSpec = new SDL_AudioSpec();
       m_wantedSpec->freq = m_decoderContext->sample_rate;
       m_wantedSpec->format = AUDIO_S16SYS;
       m_wantedSpec->channels = m_decoderContext->channels;
       m_wantedSpec->silence = 0;
       m_wantedSpec->samples = 4096;
       m_wantedSpec->callback = AudioCallback; 
    }
}

void AudioPlayback::DecodeFrame()
{
}

void AudioPlayback::StartPlayback()
{
}
