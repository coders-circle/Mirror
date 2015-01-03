#include "client/MediaStream/AudioStream.h"

void AudioStream::Initialize(int samplerate, int bitrate, int channel)
{
	m_codecID = AV_CODEC_ID_AAC;
	m_codec = avcodec_find_encoder(m_codecID);
	if (!m_codec) throw CodecMissingException();
    m_codecContext = avcodec_alloc_context3(m_codec);

    m_codecContext->bit_rate = bitrate;
    m_codecContext->sample_rate = samplerate;
    m_codecContext->channels = channel;
    m_codecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    m_codecContext->time_base = (AVRational){1, samplerate};
    m_codecContext->codec_type = AVMEDIA_TYPE_AUDIO ;
    m_codecContext->channel_layout = (channel<2 ) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
    m_codecContext->profile = FF_PROFILE_AAC_LOW;

    if(avcodec_open2(m_codecContext, m_codec,NULL) != 0)
    {
        throw Exception("failed to open the codec");
    }
}

unsigned int AudioStream::AllocateNewPacket(void)
{
	m_encodedFrames.resize(m_encodedFrames.size() + 1);
    return m_encodedFrames.size() - 1;
}

void AudioStream::AddFrame(uint8_t* samples, int pts)
{
	AVFrame *frame = avcodec_alloc_frame();
	frame->nb_samples = m_codecContext->frame_size;
    frame->format = m_codecContext->sample_fmt;
    frame->channel_layout = m_codecContext->channel_layout;

    int bufferSize = av_samples_get_buffer_size(NULL, m_codecContext->channels, 
    						m_codecContext->frame_size, m_codecContext->sample_fmt, 0);

    if(avcodec_fill_audio_frame(frame, m_codecContext->channels, m_codecContext->sample_fmt, 
                                (const uint8_t*)samples, bufferSize, 0) < 0)
    {
        throw Exception("failed to allocate raw sample buffer");   
    }

    frame->pts = pts;   
    AddFrame(frame);
}

void AudioStream::AddFrame(AVFrame *frame)
{
    unsigned int packetIndex = AllocateNewPacket();
    AVPacket* pkt = &m_encodedFrames[packetIndex];

    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    int packetFilled = 0;
    if(avcodec_encode_audio2(m_codecContext, pkt, frame, &packetFilled) < 0)
    {
        throw Exception("failed to encode the frame");
    }

    if(packetFilled)
    {
        
    }
}

void AudioStream::Test(void)
{
    uint8_t* samples = new uint8_t[m_codecContext->frame_size*2*m_codecContext->channels];
    float t=0;
    float tincr = 2.6;
    int j;
    for(int i=0; i<200; i++)
    {
        for(j=0; j<m_codecContext->frame_size; j++)
        {
            samples[2*j] = (int)(sin(t) * 10000);
            samples[2*j+1] = samples[2*j];
            t += tincr;
        }
        this->AddFrame(samples, i);
    }

    FILE* fp = fopen("encodetest.aac", "wb");
    for (int i = 0; i < m_encodedFrames.size(); i++)
    {
         fwrite(m_encodedFrames[i].data, 1, m_encodedFrames[i].size, fp);
    }

    fclose(fp);

}