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
                // wait for decoder to finish the frame
                while (!m_decodedFrameLock.try_lock())
                    ;
                    
                    //std::cout << "start playback waiting..." << std::endl;
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
                    av_frame_free(&m_decodedFrames[0]);
                    this->EraseDecodedFrameFromHead();
                    
                    timeElapsed = 0;
                    m_decodedFrameLock.unlock();
                }
                else
                {
                    m_decodedFrameLock.unlock();
                    //
                    
                        
                    if (m_encodedPacketLock.try_lock()){
                        if (m_encodedPackets.size() > 0)
                        {
                            this->AddPacket(m_encodedPackets[0]);
                            av_free_packet(m_encodedPackets[0]);
                            this->EraseEncodedPacketFromHead();
                        }
                        m_encodedPacketLock.unlock();
                    }
                    else
                    {
                        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
                    }
                }
            }
            timeElapsed = t.Elapsed();
            boost::this_thread::sleep(boost::posix_time::milliseconds(50));
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
