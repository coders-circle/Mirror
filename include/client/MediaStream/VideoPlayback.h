#pragma once

#include "client/MediaStream/VideoStream.h"
#include "client/UI/FrameRenderer.h"


class VideoPlayback : public VideoStream
{
public:
    VideoPlayback()
    {
        this->InitializeDecoder();
        m_frameRenderer = 0;
    }
    void SetFrameRenderer(FrameRenderer* frameRenderer)
    {
        m_frameRenderer = frameRenderer;
    }
    void FeedEncodedStream(unsigned char* encodedStream, int streamSize)
    {
        // TODO: implement mutex
        this->AddEncodedDataStream(encodedStream, streamSize);
    }
    void FrameAvailabilityTest()
    {
        // TODO: implement mutex
        if (m_encodedDataStream.size() >= IP_VIDEO_STREAM_BUFFER_SIZE) // if enough data is available
        {
            AVFrame *frame;
            uint8_t ipBuffer[IP_VIDEO_STREAM_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
            AVPacket pkt;
            av_init_packet(&pkt);

            // set end of buffer to 0 
            // this ensures that no overreading happens for damaged streams
            memset(ipBuffer + IP_VIDEO_STREAM_BUFFER_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

            memcpy(ipBuffer, m_encodedDataStream.data(), IP_VIDEO_STREAM_BUFFER_SIZE);
            pkt.size = m_encodedDataStream.size();
            pkt.data = ipBuffer;
            while (pkt.size > 0)
            {
                int bytesRead = this->AddProgressivePacket(&pkt);
                pkt.size -= bytesRead;
                pkt.data += bytesRead;
            }

            this->EraseEncodedDataStreamFromHead(IP_VIDEO_STREAM_BUFFER_SIZE);
        }
    }
private:
    FrameRenderer* m_frameRenderer;
    int m_fw, m_fh;
    int m_fps;
};