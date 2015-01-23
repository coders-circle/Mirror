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
        while (!m_playbackStopped)
        {
            if (m_newFrameAvailable)
            {
                m_newFrameAvailable = false;
                if (!m_frameRenderer)
                {
                    m_frameRenderer = new FrameRenderer;
                    m_frameRenderer->Set(m_fixed, m_x, m_y, m_fw, m_fh);
                    m_frameRenderer->Show();
                    if (m_fps != 0) m_frameDelay = 1000 / m_fps;
                    else m_frameDelay = 10;
                }
                unsigned char* rgbData = this->GetRawRGBData();
                if (rgbData) m_frameRenderer->SetRGBData(rgbData);
            }
            else
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(m_frameDelay));
                if (!m_frameDelay)
                {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
                }
            }
        }
        //while (!m_playbackStopped)
        //{
        //    if (timeElapsed >= m_frameDelay)
        //    {
        //        while (!m_decodedFrameLock.try_lock())
        //        {

        //        }
        //            
        //            //std::cout << "start playback waiting..." << std::endl;
        //        if (m_decodedFrames.size() > 0)
        //        {
        //            t.Reset();
        //            if (!m_frameRenderer)
        //            {
        //                m_frameRenderer = new FrameRenderer;
        //                m_frameRenderer->Set(m_fixed, m_x, m_y, m_fw, m_fh);
        //                m_frameRenderer->Show();
        //                if (m_frameDelay == 0 && m_fps != 0)
        //                {
        //                    m_frameDelay = 1000000 / m_fps;
        //                }
        //            }
        //            unsigned char* rgbData = this->GetRawRGBData(0);
        //            if (rgbData)
        //            {
        //                m_frameRenderer->SetRGBData(rgbData);
        //            }
        //            av_frame_free(&m_decodedFrames[0]);
        //            this->EraseDecodedFrameFromHead();
        //            timeElapsed = 0;
        //            m_decodedFrameLock.unlock();
        //        }
        //        else
        //        {
        //            m_decodedFrameLock.unlock();
        //            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        //            /*if (m_encodedPacketLock.try_lock()){
        //                if (m_encodedPackets.size() > 0)
        //                {
        //                    this->AddPacket(m_encodedPackets[0]);
        //                    av_free_packet(m_encodedPackets[0]);
        //                    this->EraseEncodedPacketFromHead();
        //                }
        //                m_encodedPacketLock.unlock();
        //            }*/
        //        }
        //    }
        //    timeElapsed = t.Elapsed();
        //    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        //}
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
        for (uint32_t sourceId:streamer.GetSources())
        {
            // Handle source sourceID here
            size_t len = streamer.GetPacket(sourceId, &pdata, av_malloc);
            if (len > 0)
            {
                //this->AddPacket(pdata, len);
                if (this->DecodeToFrame(pdata, len))
                {
                    m_newFrameAvailable = true;
                }
            }
        }
    }
private:
    FrameRenderer *m_frameRenderer;
    GtkWidget* m_fixed;
    int m_x, m_y;

    unsigned int m_frameDelay;
    boost::thread m_playbackThread;
    bool m_playbackStopped;
    bool m_newFrameAvailable;
};
