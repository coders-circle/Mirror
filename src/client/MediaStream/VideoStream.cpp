#include "common/common.h"
#include "client/MediaStream/VideoStream.h"



VideoStream::VideoStream() 
{
    m_codecID = AV_CODEC_ID_H264;
    m_RGB24ToYUP420PConverterContext = 0;
    m_YUV420PToRGB24ConverterContext = 0;
    m_fw = 0;
    m_fh = 0;
    m_fps = 0;
    m_decodedFrame = av_frame_alloc();
    m_encodedPacket = new AVPacket;
    av_init_packet(m_encodedPacket);
}

VideoStream::~VideoStream()
{
    if (m_RGB24ToYUP420PConverterContext)
    {
        sws_freeContext(m_RGB24ToYUP420PConverterContext);
    }
    if (m_YUV420PToRGB24ConverterContext)
    {
        sws_freeContext(m_YUV420PToRGB24ConverterContext);
    }
}

void VideoStream::InitializeDecoder()
{
    this->SetupDecoder();
    this->OpenCodec(m_decoderContext, m_decoder);
}


void VideoStream::InitializeEncoder(int w, int h, int fps, int bitrate)
{
    this->SetupEncoder();
    AVRational avrat = { 1, fps };
    m_encoderContext->bit_rate = bitrate;
    m_encoderContext->width = w;
    m_encoderContext->height = h;
    m_encoderContext->time_base = avrat;
    m_encoderContext->gop_size = 30;
    m_encoderContext->max_b_frames = 0;
    m_encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
    if (m_codecID == AV_CODEC_ID_H264)
    {
        av_opt_set(m_encoderContext->priv_data, "tune", "zerolatency", 0);
    }
    this->OpenCodec(m_encoderContext, m_encoder);
    m_RGB24ToYUP420PConverterContext = sws_getContext(m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_RGB24, m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    m_fw = w;
    m_fh = h;
    m_fps = fps;
}

AVFrame* VideoStream::DecodeToFrame(AVPacket* pkt)
{
    //boost::lock_guard<boost::mutex> guard(m_decodedFrameLock);
    while (!m_decodedFrameLock.try_lock())
        std::cout << "DecodeToFrame waiting..." << std::endl;
    int framePresent = 0;
    if (avcodec_decode_video2(m_decoderContext, m_decodedFrame, &framePresent, pkt) < 0)
    {
        m_decodedFrameLock.unlock();
        throw FailedToDecode();
    }
    if (framePresent && m_decodedFrame->width != 0)
    {
        m_decodedFrameLock.unlock();
        if (m_rawData.size() == 0)
        {
            m_fw = m_decodedFrame->width;
            m_fh = m_decodedFrame->height;
            this->AllocateRawData(m_fw*m_fh * 3);
            m_fps = m_decoderContext->time_base.den
                / (m_decoderContext->time_base.num*m_decoderContext->ticks_per_frame);
            m_YUV420PToRGB24ConverterContext = sws_getContext(m_fw, m_fh,
                AV_PIX_FMT_YUV420P, m_fw, m_fh, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
        }
    }
    else m_decodedFrameLock.unlock();
    return m_decodedFrame;
}

AVFrame* VideoStream::DecodeToFrame(uint8_t *packetData, int packetSize)
{
    AVPacket* pkt = new AVPacket();
    av_init_packet(pkt);
    if (av_packet_from_data(pkt, packetData, packetSize) < 0)
    {
        return 0;
    }
    AVFrame* frame = this->DecodeToFrame(pkt);
    av_free_packet(pkt);
    return frame;
}

AVPacket* VideoStream::EncodeToPacket(AVFrame* frame)
{
    //boost::lock_guard<boost::mutex> guard(m_encodedPacketLock);
    while (!m_encodedPacketLock.try_lock())
        std::cout << "EncodeToPacket waiting..." << std::endl;
    if (m_encodedPacket->size)
    {
        av_free_packet(m_encodedPacket);
    }
    av_init_packet(m_encodedPacket);
    m_encodedPacket->data = NULL;
    m_encodedPacket->size = 0;
    int packetFilled = 0;
    if (avcodec_encode_video2(m_encoderContext, m_encodedPacket, frame, &packetFilled) < 0)
    {
        m_encodedPacketLock.unlock();
        throw FailedToEncode();
    }
    m_encodedPacketLock.unlock();
    return m_encodedPacket;
}

AVPacket* VideoStream::EncodeToPacket(uint8_t* rgb24Data, uint64_t pts)
{
    AVFrame *frame = av_frame_alloc();
    frame->width = m_encoderContext->width;
    frame->height = m_encoderContext->height;
    frame->format = m_encoderContext->pix_fmt;
    frame->pts = pts;
    if (av_image_alloc(frame->data, frame->linesize, m_encoderContext->width,
        m_encoderContext->height, m_encoderContext->pix_fmt, 32) < 0)
    {
        throw Exception("failed to allocate raw picture buffer");
    }
    // Convert RGB data to YUV before adding the frame for encoding
    uint8_t * inData[1] = { rgb24Data };                    // RGB24 have only one plane
    int inLinesize[1] = { 3 * m_encoderContext->width };    // RGB stride
    sws_scale(m_RGB24ToYUP420PConverterContext, inData, inLinesize, 0, 
        frame->height, frame->data, frame->linesize);
    AVPacket* pkt = this->EncodeToPacket(frame);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
    return pkt;
}

unsigned char* VideoStream::GetRawRGBData()
{
    while (!m_decodedFrameLock.try_lock())
        std::cout << "GetRawRGBData waiting..." << std::endl;
    if (!m_decodedFrame) 
    {
        m_decodedFrameLock.unlock();
        return 0;
    }
    if(m_decodedFrame->width == 0)
    {
        m_decodedFrameLock.unlock();
        return 0;
    }
    int w = m_decodedFrame->width;
    int h = m_decodedFrame->height;
    if (w != 0)
    {
        AVFrame* yuvFrame = m_decodedFrame;
        AVPicture *rgbFrame = new AVPicture;
        avpicture_alloc(rgbFrame, AV_PIX_FMT_RGB24, w, h);
        sws_scale(m_YUV420PToRGB24ConverterContext, yuvFrame->data,
            yuvFrame->linesize, 0, h, rgbFrame->data, rgbFrame->linesize);
        m_decodedFrameLock.unlock();
        memcpy(m_rawData.data(), rgbFrame->data[0], w*h * 3);
        avpicture_free(rgbFrame);
        delete rgbFrame;
    }
    else m_decodedFrameLock.unlock();
    return m_rawData.data();
}