#pragma once

#include "client/MediaStream/MediaStream.h"

class VideoStream :public MediaStream
{
public:
    AVFrame* DecodeToFrame(AVPacket* pkt)
    {
        boost::lock_guard<boost::mutex> locker(m_decodedFrameLock);
        /*while (!m_decodedFrameLock.try_lock())
            std::cout << "DecodeToFrame waiting..."<< std::endl;*/
        int framePresent = 0;
        if (avcodec_decode_video2(m_decoderContext, m_decodedFrame, &framePresent, pkt) < 0)
        {
            //m_decodedFrameLock.unlock();
            throw FailedToDecode();
        }
        if (framePresent && m_decodedFrame->width != 0)
        {
            if (m_rawData.size() == 0)
            {
                m_fw = m_decodedFrame->width;
                m_fh = m_decodedFrame->height;
                m_rawData.resize(m_fw*m_fh * 4);
                m_fps = m_decoderContext->time_base.den 
                    / (m_decoderContext->time_base.num*m_decoderContext->ticks_per_frame);
                m_YUV420PToBGRAConverterContext = sws_getContext(m_fw, m_fh,
                    AV_PIX_FMT_YUV420P, m_fw, m_fh, AV_PIX_FMT_BGRA, SWS_BICUBIC, 0, 0, 0);
            }
            //m_decodedFrameLock.unlock();
        }
        //else m_decodedFrameLock.unlock();
        return m_decodedFrame;
    }
    AVFrame* DecodeToFrame(uint8_t *packetData, int packetSize)
    {
        AVPacket* pkt = new AVPacket();
        /*if (!av_new_packet(pkt, packetSize))
        {
            throw FailedToDecode();
        }*/
        av_init_packet(pkt);
        pkt->data = packetData;
        pkt->size = packetSize;
        /*if (av_packet_from_data(pkt, packetData, packetSize) < 0)
        {
            return 0;
        }*/
        AVFrame* frame = this->DecodeToFrame(pkt);
        av_free_packet(pkt);
        return frame;
    }
    AVPacket* EncodeToPacket(AVFrame* frame)
    {
        boost::lock_guard<boost::mutex> locker(m_encodedPacketLock);
        /*while (!m_encodedPacketLock.try_lock())
            std::cout << "EncodeToPacket waiting..."<<std::endl;*/
        if (m_encodedPacket->size) 
        { 
            av_free_packet(m_encodedPacket); 
        }
        av_init_packet(m_encodedPacket);
        m_encodedPacket->data = 0;
        m_encodedPacket->size = 0;
        int packetFilled = 0;
        if (avcodec_encode_video2(m_encoderContext, m_encodedPacket, frame, &packetFilled) < 0)
        {
            //m_encodedPacketLock.unlock();
            throw FailedToEncode();
        }
        // m_encodedPacketLock.unlock();
        m_newPacketAvailable = true;
        return m_encodedPacket;
    }
    AVPacket* EncodeToPacket(uint8_t* rgb24Data, uint64_t pts)
    {
        AVFrame *frame = av_frame_alloc();
        frame->width  = m_encoderContext->width;
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
        sws_scale(m_RGB24ToYUP420PConverterContext, inData, inLinesize, 0, frame->height, frame->data, frame->linesize);

        AVPacket* pkt = this->EncodeToPacket(frame);
        av_freep(&frame->data[0]);
        //av_frame_free(&frame);
        avcodec_free_frame(&frame);
        return pkt;
    }
    unsigned char* GetRawBGRAData()
    {
        boost::lock_guard<boost::mutex> locker(m_decodedFrameLock);
        /*while (!m_decodedFrameLock.try_lock())
            std::cout << "GetRawRGBData waiting..." << std::endl;*/
        if (!m_decodedFrame)
        {
            //m_decodedFrameLock.unlock();
            return 0;
        }
        if (m_decodedFrame->width == 0)
        {
            //m_decodedFrameLock.unlock();
            return 0;
        }
        int w = m_decodedFrame->width;
        int h = m_decodedFrame->height;
        AVFrame* yuvFrame = m_decodedFrame;
        //AVPicture *bgraFrame = new AVPicture;
        //avpicture_alloc(bgraFrame, AV_PIX_FMT_BGRA, w, h);
        //sws_scale(m_YUV420PToBGRAConverterContext, yuvFrame->data, yuvFrame->linesize, 0, h, bgraFrame->data, bgraFrame->linesize);
        uint8_t * outData[1] = { m_rawData.data() };                    // RGB24 have only one plane
        int outLinesize[1] = { 4 * w };    // RGB stride
        sws_scale(m_YUV420PToBGRAConverterContext, yuvFrame->data, yuvFrame->linesize, 0, h, outData, outLinesize);
        //m_decodedFrameLock.unlock();
        //memcpy(m_rawData.data(), bgraFrame->data[0], w*h * 4);
        //avpicture_free(bgraFrame);
        //delete bgraFrame;
        //else m_decodeDdFrameLock.unlock();
        return m_rawData.data();
    }

    VideoStream();
    ~VideoStream();

    // Initialize the video stream for decoding process
    void InitializeDecoder();

    // Initialize the video stream for encoding process
    void InitializeEncoder(int w, int h, int fps = 15, int bitrate = 200000);

protected:
    // scaler context to convert 24-bit RGB to YUV420P color format
    SwsContext* m_RGB24ToYUP420PConverterContext;
    // scaler context to convert YUV420P to 24-bit RGB color format
    SwsContext* m_YUV420PToRGB24ConverterContext;

    // scaler context to convert 32-bit BGRA to YUV420P color format
    SwsContext* m_BGRAToYUP420PConverterContext;
    // scaler context to convert YUV420P to 32-bit BGRA color format
    SwsContext* m_YUV420PToBGRAConverterContext;

    // frame width and height
    int m_fw, m_fh;

    // frame rate
    int m_fps;
};
