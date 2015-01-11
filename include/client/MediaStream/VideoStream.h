#pragma once

#include "client/MediaStream/MediaStream.h"
#include "client/MediaStream/RtpStreamer.h"

#define IP_VIDEO_STREAM_BUFFER_SIZE 4096


class Client;
class VideoStream :public MediaStream
{
public:
    AVFrame* DecodeToFrame(AVPacket* pkt);
    AVFrame* DecodeToFrame(uint8_t *packetData, int packetSize);
    AVPacket* EncodeToPacket(AVFrame* frame);
    AVPacket* EncodeToPacket(uint8_t* rgb24Data, uint64_t pts);
    unsigned char* GetRawRGBData();

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

    // frame width and height
    int m_fw, m_fh;
    int m_fps;
};
