#pragma once

#include "client/MediaStream/MediaStream.h"
#include "client/MediaStream/RtpStreamer.h"

#define IP_VIDEO_STREAM_BUFFER_SIZE 4096


class Client;
class VideoStream :public MediaStream
{
public:
    

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
