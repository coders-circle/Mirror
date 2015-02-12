#pragma once

#include "common/common.h"

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}

#include <iostream>
#include <vector>

#ifndef _WIN32
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_copy(dst, src) av_image_copy(dst->data, dst->linesize, (const uint8_t**)src->data, src->linesize, (AVPixelFormat)src->format, src->width, src->height)
#endif

class CodecNotFound :public Exception
{
public:
    CodecNotFound() :Exception("can't find the suitable codec"){}
};

class FailedToEncode : public Exception
{
public:
    FailedToEncode() :Exception("failed to encode the frame"){}
};

class FailedToDecode :public Exception
{
public:
    FailedToDecode() :Exception("failed to decode the frame"){}
};


//-----------------------------------------------------
// A general class representing the Audio/Video Stream
//
// General concept
// ---------------
// Frames are used to store raw data,
// Packets are used to store encoded data.
//
// Frames are encoded into packets
// and, packets are decoded to frames
//-----------------------------------------------------
class MediaStream
{
public:
    AVPacket* GetPacket()
    {
        while (!m_newPacketAvailable)
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        boost::lock_guard<boost::mutex> locker(m_encodedPacketLock);
        m_newPacketAvailable = false;
        /*if (m_encodedPacketBuffer->size)
        {
            av_free_packet(m_encodedPacketBuffer);
        }
        av_init_packet(m_encodedPacket);
        av_copy_packet(m_encodedPacketBuffer, m_encodedPacket);*/
        return m_encodedPacket;
    }
protected:
    MediaStream();
    ~MediaStream();

    // Setup the stream for encoding process
    void SetupEncoder();

    // Setup the stream for decoding process
    void SetupDecoder();

    // Open the codec for the context setup
    void OpenCodec(AVCodecContext* codecContext, AVCodec* codec);
    
    //--------------------------------------------------
    //common variables required for a audio/video stream
    //--------------------------------------------------

    // codec id for the media: 
    // AV_CODEC_ID_AAC for audio,
    // AV_CODEC_ID_H264 for video
    AVCodecID m_codecID;  

    AVCodec *m_encoder;                 // Encoder Handle
    AVCodecContext *m_encoderContext;   // Encoder Context


    AVCodec *m_decoder;                 // Decoder Handle
    AVCodecContext *m_decoderContext;   // Decoder Context

    // used to store raw data
    // typical use:
    // to return raw data after decoding,
    std::vector<uint8_t> m_rawData;

    boost::mutex m_decodedFrameLock;
    boost::mutex m_encodedPacketLock;

    AVPacket* m_encodedPacket;
    AVPacket* m_encodedPacketBuffer;
    AVFrame* m_decodedFrame;

    bool m_newPacketAvailable;
    bool m_newFrameAvailable;
};
