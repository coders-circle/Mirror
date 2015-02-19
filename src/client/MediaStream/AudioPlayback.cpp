#include <common/common.h>
#include <client/MediaStream/AudioPlayback.h>

boost::mutex AudioPlayback::m_queueMutex;
std::queue<AVPacket> AudioPlayback::m_incomingPackets;

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

    if (!m_obtainedSpec)
        m_obtainedSpec = new SDL_AudioSpec();
    if(!m_wantedSpec)
    {
        m_wantedSpec = new SDL_AudioSpec();
        m_wantedSpec->freq = m_decoderContext->sample_rate;
        m_wantedSpec->format = AUDIO_S16SYS;
        m_wantedSpec->channels = m_decoderContext->channels;
        m_wantedSpec->silence = 0;
        m_wantedSpec->samples = 4096;
        m_wantedSpec->callback = AudioPlayback::AudioCallback; 
        m_wantedSpec->userdata = m_decoderContext;
    }
}

size_t AudioPlayback::DecodeFrame(AVCodecContext* codecCtx, uint8_t* audio_buf)
{
    if (!codecCtx)
        return -1;

    boost::lock_guard<boost::mutex> guard(m_queueMutex);
    AVFrame* frame = NULL;
    for(;;)
    {
        AVPacket& packet = m_incomingPackets.front();
        while (packet.size > 0)
        {
            if (!frame)
            {
                if (!(frame = avcodec_alloc_frame()))
                {
                    std::cout << "Couldn't allocate frame" << std::endl;
                    return -1;
                }
            }
            else
                avcodec_get_frame_defaults(frame);

            int got_frame = 0;
            int len = avcodec_decode_audio4(codecCtx, frame, &got_frame, &packet);
            if (len < 0)
            {
                std::cout << "Error while decoding" << std::endl;
                return -1;
            }

            if (got_frame)
            {
                memcpy(audio_buf, frame->data[0], len);
                if (frame)
                    avcodec_free_frame(&frame);
                return len;
            }
            packet.size -= len;
            packet.data += len;
            packet.dts = packet.pts = AV_NOPTS_VALUE;
        }
        if (frame)
        {
            avcodec_free_frame(&frame);
            frame = NULL;
        }
        if (packet.data)
            av_free_packet(&packet);
        if (m_incomingPackets.size() == 0)
            return -1;
        m_incomingPackets.pop();
    }
}

void AudioPlayback::StartPlayback()
{
    Initialize();
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
    auto adevice = SDL_OpenAudioDevice(NULL, 0, m_wantedSpec, m_obtainedSpec, 0);

    SDL_PauseAudioDevice(adevice, 0);
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        SDL_Delay(1);
    }
}

void AudioPlayback::AudioCallback(void *userdata, uint8_t *stream, int len)
{
    boost::lock_guard<boost::mutex> guard(m_queueMutex);
    int len1, audio_size;

    AVCodecContext* context = (AVCodecContext*)userdata;
    static uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2];
    static uint32_t audio_buf_size = 0;
    static uint32_t audio_buf_index = 0;

    while (len > 0)
    {
        if (audio_buf_index >= audio_buf_size)
        {
            // Get more data
            audio_size = DecodeFrame(context, audio_buf);
            if (audio_size < 0)
            {
                audio_buf_size = 1024;
                memset(audio_buf, 0, audio_buf_size);
            }
            else
                audio_buf_size = audio_size;
            audio_buf_index = 0;
        }
        len1 = audio_buf_size - audio_buf_index;
        if (len1 > len)
            len1 = len;
        memcpy(stream, (uint8_t*)audio_buf + audio_buf_index, len1);
        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
    
}
