#pragma once

#include <common/common.h>
#include "common/Exception.h"

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
protected:
    MediaStream();
    ~MediaStream();

    // Setup the stream for encoding process
    void SetupEncoder();

    // Setup the stream for decoding process
    void SetupDecoder();

    // Open the codec for the context setup
    void OpenCodec(AVCodecContext* codecContext, AVCodec* codec);

    // Allocates memory for a new encoded packet
    unsigned int AllocateNewEndodedPacket();

    // Allocates memory for a new decoded frame
    unsigned int AllocateNewDecodedFrame();

    // Removes the given no. of packets from the begining of the packets
    void EraseEncodedPacketFromHead(unsigned int noOfPackets = 1);

    // Removes the given no. of frames from the begining of the frames
    void EraseDecodedFrameFromHead(unsigned int noOfFrames = 1);

    // Allocates memory for the given no. of bytes to add into the raw data
    unsigned int AllocateRawData(unsigned int noOfBytesToAdd = 1);

    // Removes the given no. of bytes from the begining of the raw data
    void EraseRawDataFromHead(unsigned int noOfBytesToRemove = 1);

    // Allocates memory for the given no. of bytes to add into the encoded data stream
    // used exclusively for continious decoding process
    unsigned int AllocateEncodedDataStream(unsigned int noOfBytesToAdd);

    // Removes the given no. of bytes from the begining of the encoded stream
    // used exclusively for continious decoding process
    void EraseEncodedDataStreamFromHead(unsigned int noOfBytesToAdd);

    // Adds data stream to the decoding queue
    // used exclusively for continious decoding process
    void AddEncodedDataStream(unsigned char* encodedStream, unsigned int streamSizeInBytes);
    
    
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

    std::vector<AVPacket*>  m_encodedPackets;   // Encoded Packets
    std::vector<AVFrame*>   m_decodedFrames;    // Decoded Frames

    // used to store raw data
    // typical use:
    // to return raw data after decoding,
    std::vector<uint8_t> m_rawData;

    // used to store encoded data stream which is then
    // queued for the decoding process
    // used exclusive for continious decoding process
    std::vector<uint8_t> m_encodedDataStream;


    // mutex to lock encoded frames
    boost::mutex m_frameLock;

    boost::mutex m_decodedFrameLock;
    boost::mutex m_encodedPacketLock;
};
