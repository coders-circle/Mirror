#pragma once

#include "client/MediaStream/MediaStream.h"
#include "client/MediaStream/RtpStreamer.h"

#define IP_VIDEO_STREAM_BUFFER_SIZE 4096


class Client;
class VideoStream :public MediaStream
{
public:
    AVFrame* DecodeToFrame(AVPacket* pkt)
    {
        while (!m_decodedFrameLock.try_lock())
            std::cout << "DecodeToFrame waiting..."<< std::endl;
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
    AVFrame* DecodeToFrame(uint8_t *packetData, int packetSize)
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
    AVPacket* EncodeToPacket(AVFrame* frame)
    {
        while (!m_encodedPacketLock.try_lock())
            std::cout << "EncodeToPacket waiting..."<<std::endl;
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
        av_frame_free(&frame);
        return pkt;
    }
    unsigned char* GetRawRGBData()
    {
        while (!m_decodedFrameLock.try_lock())
            std::cout << "GetRawRGBData waiting..."<<std::endl;
        if (!m_decodedFrame || m_decodedFrame->width == 0)
            return 0;
        int w = m_decodedFrame->width;
        int h = m_decodedFrame->height;
        if (w != 0)
        {
            AVFrame* yuvFrame = m_decodedFrame;
            AVPicture *rgbFrame = new AVPicture;
            avpicture_alloc(rgbFrame, AV_PIX_FMT_RGB24, w, h);
            sws_scale(m_YUV420PToRGB24ConverterContext, yuvFrame->data, yuvFrame->linesize, 0, h, rgbFrame->data, rgbFrame->linesize);
            m_decodedFrameLock.unlock();
            memcpy(m_rawData.data(), rgbFrame->data[0], w*h * 3);
            avpicture_free(rgbFrame);
            delete rgbFrame;
        }
        else m_decodedFrameLock.unlock();
        return m_rawData.data();
    }

    

    VideoStream();
    ~VideoStream();

    // Initialize the video stream for decoding process
    void InitializeDecoder();

    // Initialize the video stream for encoding process
    void InitializeEncoder(int w, int h, int fps = 15, int bitrate = 100000);

    // Add a packet and decode it to frame
    void AddPacket(AVPacket* pkt);

    void AddPacket(uint8_t* packetData, int packetSize);

    // Get 24-bit raw RGB data from the given decoded frame
    unsigned char* GetRawRGBData(unsigned int decodedFrameIndex);
    
    // Add 24-bit raw RGB frame and encode into packet
    // pts = presentation time stamp
    void AddFrame(uint8_t* rgb24Data, uint64_t pts);

    // Add a frame and encode into packet
    void AddFrame(AVFrame *frame);

    //int AddProgressivePacket(AVPacket* pkt);

    // @@@@@@@@@@@@@@@@@@
    // TEMPORARY fuction
    // used for testing
    void Test(); 

    // @@@@@@@@@@@@@@@@@@@
    // TEMPORARY function
    // Test Encode to File
    void Encode();

protected:
    // scaler context to convert 24-bit RGB to YUV420P color format
    SwsContext* m_RGB24ToYUP420PConverterContext;
    // scaler context to convert YUV420P to 24-bit RGB color format
    SwsContext* m_YUV420PToRGB24ConverterContext;

    // frame width and height
    int m_fw, m_fh;
    int m_fps;
};
