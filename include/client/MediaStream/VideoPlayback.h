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
        m_newFrameAvailable = false;
    }
    void Set(GtkWidget* fixed, int x, int y)
    {
        m_fixed = fixed;
        m_x = x;
        m_y = y;
        m_frameDelay = 0;
        
    }

    void StartPlayback()
    {
        m_playbackStopped = false;
        Timer timeElapsed;
        int frameCounter = 0;
        while (!m_playbackStopped)
        {
            timeElapsed.Reset();
            if (m_newFrameAvailable)
            {
                m_newFrameAvailable = false;
                if (!m_frameDelay)
                {
                    m_frameRenderer = new FrameRenderer;
                    m_frameRenderer->Set(m_fixed, m_x, m_y, m_fw, m_fh);
                    m_frameRenderer->Show();
                    if (m_fps != 0) m_frameDelay = 1000000L / m_fps;
                    else m_frameDelay = 10000L;
                }
                unsigned char* bgraData = this->GetRawBGRAData();
                if (bgraData) m_frameRenderer->SetBGRAData(bgraData);
                ++frameCounter;
                if (frameCounter % 10 == 0)
                    std::cout << frameCounter << std::endl;
            }
            else
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(timeElapsed.ElapsedMilliSecs()));
                /*if (timeElapsed.ElapsedMicroSecs() < (m_frameDelay + 2000L))
                {
                    boost::this_thread::sleep(boost::posix_time::milliseconds((m_frameDelay-timeElapsed.ElapsedMicroSecs()))/1000L);
                }*/
                if (!m_frameDelay)
                {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                }
            }
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

    void ReceiveRtp(RtpStreamer& streamer)
    {
        uint8_t* pdata = 0;
        size_t len = streamer.GetPacket(0, &pdata, av_malloc);
        if (len > 0)
        {
            try
            {
                if (this->DecodeToFrame(pdata, len))
                {
                    m_newFrameAvailable = true;
                }
            }
            catch (FailedToDecode)
            {
                std::cout << "packet lost :/" << std::endl;
            }
        }
    }
private:
    FrameRenderer *m_frameRenderer;
    GtkWidget* m_fixed;
    int m_x, m_y;

    long long m_frameDelay;
    boost::thread m_playbackThread;
    bool m_playbackStopped;
    bool m_newFrameAvailable;
};
