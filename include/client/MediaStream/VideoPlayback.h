#pragma once


#include "client/MediaStream/VideoStream.h"
#include "client/UI/FrameRenderer.h"

#include "common/Timer.h"


class VideoPlayback : public VideoStream
{
public:
    VideoPlayback()
    {
        this->InitializeDecoder();
        m_frameRenderer = 0;
    }
    void Set(GtkWidget* fixed, int x, int y)
    {
        m_fixed = fixed;
        m_x = x;
        m_y = y;
        m_frameDelay = 0;
    }

    // for progressive decoding
    void FeedEncodedStream(unsigned char* encodedStream, int streamSize)
    {
        // TODO: implement mutex
        this->AddEncodedDataStream(encodedStream, streamSize);
    }

    // test for progressive decoding
    void FrameAvailabilityTest()
    {
        // TODO: implement mutex
        if (m_encodedDataStream.size() >= IP_VIDEO_STREAM_BUFFER_SIZE) // if enough data is available
        {
            //AVFrame *frame;
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
    void StartPlayback()
    {
        m_playbackStopped = false;
        Timer t2;
        Timer t;
        unsigned timeElapsed = 0;
        t2.Reset();
        t.Reset();
        int i = 0;
        while (!m_playbackStopped)
        {
            if (timeElapsed >= m_frameDelay)
            {
                if (m_decodedFrames.size() > 0)
                {
                    t.Reset();
                    if (!m_frameRenderer)
                    {
                        m_frameRenderer = new FrameRenderer;
                        m_frameRenderer->Set(m_fixed, m_x, m_y, m_fw, m_fh);
                        m_frameRenderer->Show();
                        if (m_frameDelay == 0 && m_fps != 0)
                        {
                            m_frameDelay = 1000000 / m_fps;
                        }
                    }
                    unsigned char* rgbData = this->GetRawRGBData(0);
                    if (rgbData)
                    {
                        m_frameRenderer->SetRGBData(rgbData);
                    }
                    this->EraseDecodedFrameFromHead();
                    ++i;
                    if (i == 228)
                        std::cout << t2.Elapsed()/1000000.0f <<std::endl;
                    timeElapsed = 0;
                    
                }
                else
                {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                }
            }
            //boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            timeElapsed = t.Elapsed();
            //t.Start();
            //std::cout << timeElapsed << " " << m_frameDelay << std::endl;
            //boost::this_thread::sleep(boost::posix_time::milliseconds(60));
        }
    }
    void StartPlaybackAsync()
    {
        m_playbackThread = boost::thread(boost::bind(&VideoPlayback::StartPlayback, this));
    }
    void StopPlayback()
    {

    }
    void FrameSyncronizer()
    {

    }
private:
    FrameRenderer *m_frameRenderer;
    GtkWidget* m_fixed;
    int m_x, m_y;

    unsigned int m_frameDelay;

    boost::thread m_playbackThread;
    bool m_playbackStopped;
};