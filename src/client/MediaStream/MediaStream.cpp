#include "common/common.h"
#include "client/MediaStream/MediaStream.h"

MediaStream::MediaStream()
{
    m_encoder = 0;
    m_decoder = 0;
    m_encoderContext = 0;
    m_decoderContext = 0;

    m_encodedPacketBuffer = new AVPacket;
    av_init_packet(m_encodedPacketBuffer);

    m_decodedFrame = av_frame_alloc();
    m_encodedPacket = new AVPacket;
    av_init_packet(m_encodedPacket);

    m_newPacketAvailable = false;
    m_newFrameAvailable = false;
}

MediaStream::~MediaStream()
{
    if (m_encoderContext)
    {
       // avcodec_free_context(&m_encoderContext);
        avcodec_close(m_encoderContext);
        av_freep(&m_encoderContext->extradata);
        av_freep(&m_encoderContext);
    }
    if (m_decoderContext)
    {
        //avcodec_free_context(&m_decoderContext);
        avcodec_close(m_decoderContext);
        av_freep(&m_decoderContext->extradata);
        av_freep(&m_decoderContext);
    }
}

void MediaStream::SetupEncoder()
{
    m_encoder = avcodec_find_encoder(m_codecID);
    if (!m_encoder)
    {
        throw CodecNotFound();
    }
    m_encoderContext = avcodec_alloc_context3(m_encoder);
    if (!m_encoderContext)
    {
        throw Exception("can't allocate the encoder context");
    }
}

void MediaStream::SetupDecoder()
{
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
}

void MediaStream::OpenCodec(AVCodecContext* codecContext, AVCodec* codec)
{
    if (avcodec_open2(codecContext, codec, NULL) < 0)
    {
        throw Exception("failed to open the codec");
    }
}

#ifdef _WIN32
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#endif
