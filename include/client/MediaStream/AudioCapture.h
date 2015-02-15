#pragma once
#include "MediaStream.h"

class AudioCapture
{
public:    
    void StartCapturing()
    {
        std::fstream f;
        f.open("test.wav", std::ios::out | std::ios::binary);
        Initialize();
        while (av_read_frame(m_decFormatCtx, &m_decPacket) >= 0)
        {
            //if (m_decPacket.stream_index != m_decAudioIndex)
            //    continue;
            if (!m_decFrame)
            {
                if (!(m_decFrame = avcodec_alloc_frame()))
                    throw Exception("Couldn't allocate audio frame");
            }
            else
                avcodec_get_frame_defaults(m_decFrame);
            
            int got_frame = 0;
            int m_decLen = avcodec_decode_audio4(m_decCodecCtx, m_decFrame, &got_frame, &m_decPacket);
            if (m_decLen < 0)
                throw Exception("Error decoding");

            if (got_frame)
            {
                int got_output = 0;
                av_init_packet(&m_encPacket);
                m_encPacket.data = NULL;
                m_encPacket.size = 0;
                if (avcodec_encode_audio2(m_encCodecCtx, &m_encPacket, m_decFrame, &got_output) < 0)
                    throw Exception("Error encoding audio frame");
                if (got_output)
                {
                    SendPacket();
                    f.write((char*)m_encPacket.data, m_encPacket.size);
                    av_free_packet(&m_encPacket);
                }
                av_free_packet(&m_decPacket);
            }
        }
        
        f.close();
        avcodec_close(m_decCodecCtx);
        avcodec_free_frame(&m_decFrame);
        avformat_close_input(&m_decFormatCtx);

        avcodec_close(m_encCodecCtx);
        av_free(m_encCodecCtx);
    }

    void StopCapturing()
    {
    }

private:
    void Initialize()
    {
       m_decFormatCtx = avformat_alloc_context();
        AVInputFormat* ifmt = av_find_input_format("alsa");
        if (avformat_open_input(&m_decFormatCtx, "hw:0", ifmt, NULL)!=0)
            throw Exception("Couldn't open audio input stream");

        if (avformat_find_stream_info(m_decFormatCtx, NULL) < 0)
            throw Exception("Couldn't find audio stream information");
        
        int m_decAudioIndex = -1;
        for (int i=0; i<m_decFormatCtx->nb_streams; ++i)
        if (m_decFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_decAudioIndex = i;
            break;
        }
        
        if (m_decAudioIndex == -1)
            throw Exception("Couldn't find audio stream");

        m_decCodecCtx = m_decFormatCtx->streams[m_decAudioIndex]->codec;
        m_decCodec = avcodec_find_decoder(m_decCodecCtx->codec_id);

        if (!m_decCodec)
            throw Exception("Couldn't find codec");
        if (avcodec_open2(m_decCodecCtx, m_decCodec, NULL) < 0)
            throw Exception("Couldn't open codec");

        av_init_packet(&m_decPacket);
        m_decFrame = NULL;

        m_encCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
        if (!m_encCodec)
            throw Exception("Couldn't find codec");

        m_encCodecCtx = avcodec_alloc_context3(m_encCodec);
        if (!m_encCodecCtx)
            throw Exception("Couldn't allocate codec context");
        
        m_encCodecCtx->bit_rate = 64000;
        m_encCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
        m_encCodecCtx->sample_rate = 44100;
        m_encCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
        m_encCodecCtx->time_base = (AVRational){1, m_encCodecCtx->sample_rate};
        m_encCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
        m_encCodecCtx->channels = av_get_channel_layout_nb_channels(m_encCodecCtx->channel_layout);
        m_encCodecCtx->profile = FF_PROFILE_AAC_LOW;

        if (avcodec_open2(m_encCodecCtx, m_encCodec, NULL) < 0)
            throw Exception("Couldn't open codec");
    }
    void SendPacket()
    {
        std::cout << "Encoded Packet " << m_encPacket.size << std::endl;
    }
    

    AVFormatContext* m_decFormatCtx;
    AVCodecContext* m_decCodecCtx;
    AVCodec* m_decCodec;
    AVPacket m_decPacket;
    AVFrame* m_decFrame;
    int m_decLen;
    int m_decAudioIndex;

    AVCodec* m_encCodec;
    AVCodecContext* m_encCodecCtx;
    AVFrame* m_encFrame;
    AVPacket m_encPacket;
};
