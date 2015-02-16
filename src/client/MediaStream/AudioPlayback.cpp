#include <common/common.h>
#include <client/MediaStream/AudioPlayback.h>

void AudioPlayback::Initialize()
{
    av_register_all();
    m_codecID = AV_CODEC_ID_AAC;
    m_decoder = avcodec_find_decoder(m_codecID);

    if (!m_decoder)
        throw CodecNotFound();

    m_decoderContext = avcodec_alloc_context3(m_decoder);
    if (!m_decoderContext)
        throw Exception("Couldn't allocate the decoder context");

    if (avcodec_open2(m_decoderContext, m_decoder, NULL) < 0)
        throw Exception("Couldn't open codec");

    if(!m_wantedSpec)
    {
        m_wantedSpec = new SDL_AudioSpec();
        m_wantedSpec->freq = m_decoderContext->sample_rate;
        m_wantedSpec->format = AUDIO_S16SYS;
        m_wantedSpec->channels = m_decoderContext->channels;
        m_wantedSpec->silence = 0;
        m_wantedSpec->samples = 4096;
        m_wantedSpec->callback = AudioPlayback::AudioCallback; 
    }
}

void AudioPlayback::DecodeFrame(AVPacket& packet)
{
    if (!m_decoderContext)
        return;

    AVFrame* frame = NULL;
    while (packet.size > 0)
    {
        if (!frame)
        {
            if (!(frame = avcodec_alloc_frame()))
            {
                std::cout << "Couldn't allocate frame" << std::endl;
                return;
            }
        }
        else
            avcodec_get_frame_defaults(frame);

        int got_frame = 0;
        int len = avcodec_decode_audio4(m_decoderContext, frame, &got_frame, &packet);
        if (len < 0)
        {
            std::cout << "Error while decoding" << std::endl;
            return;
        }

        if (got_frame)
        {
            boost::lock_guard<boost::mutex> guard(m_queueMutex);
            m_decodedFrames.push(frame);
            std::cout << len << std::endl;
        }
        packet.size -= len;
        packet.data += len;
        packet.dts = packet.pts = AV_NOPTS_VALUE;
    }
}

void AudioPlayback::StartPlayback()
{
    Initialize();
}
