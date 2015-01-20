#pragma once


#include "client/MediaStream/VideoStream.h"
#include "client/UI/FrameRenderer.h"

#include "common/Timer.h"

class VideoSource
{
public:
    void Set(int x, int y, int w, int h)
    {
        m_x = x;
        m_y = y;
        m_frw = w;
        m_frh = h;
    }
    VideoSource()
    {
        m_x = 0;
        m_y = 0;
        m_frw = 0;
        m_frh = 0;
    }
    uint8_t* GetRawBGRAData()
    {

    }
private:
    int m_x, m_y, m_frw, m_frh;
    std::vector<uint8_t> m_rawBGRAData;
};


class VideoPlayback : public VideoStream
{
public:
    VideoPlayback()
    {
        this->InitializeDecoder();
        m_frameRenderer = 0;
        m_newFrameAvailable = false;
    }
    void Set(GtkWidget* fixed, int x, int y, int w = 640, int h = 480)
    {
        m_fixed = fixed;
        m_frx = x;
        m_fry = y;
        m_frw = w;
        m_frh = h;
        m_frameRenderer = new FrameRenderer;
        m_frameRenderer->Set(m_fixed, m_frx, m_fry, m_frw, m_frh);
        //m_frameRenderer->Show();
        m_frameDelay = 0;
        m_rawBGRA.resize(m_frw*m_frh * 4);
    }

    void StartPlayback()
    {
        m_playbackStopped = false;
        while (!m_playbackStopped)
        {
            if (m_newFrameAvailable)
            {
                m_newFrameAvailable = false;
                if (!m_frameDelay)
                {
                    m_scw = m_fw;
                    m_sch = m_fh;
                    m_xOff = 0;
                    m_yOff = 0;
                    if (m_fw != m_frw || m_fh != m_frh)
                    {
                        // either width or height of decoded frame is 
                        // not equal to that of frame renderer
                        if (m_fw > m_frw || m_fh > m_frh)
                        {
                            // either width or height of decoded frame is 
                            // larger than that of frame renderer
                            int diffW = m_fw - m_frw;
                            int diffH = m_fh - m_frh;
                            if (diffW > diffH)
                            {
                                m_scw = m_frw;
                                m_sch = static_cast<int>(0.5f + static_cast<float>(m_fh*m_frw / m_fw));
                                if (m_sch % 2) --m_sch;
                                m_yOff = (m_frh - m_sch) >> 1;
                            }
                            else
                            {
                                m_sch = m_frh;
                                m_scw = static_cast<int>(0.5f + static_cast<float>(m_fw*m_frh / m_fh));
                                if (m_scw % 2) --m_scw;
                                m_xOff = (m_frw - m_scw) >> 1;
                            }

                        }
                        else
                        {
                            // either width or height of decoded frame is 
                            // smaller than that of frame renderer
                            int diffW = m_frw - m_fw;
                            int diffH = m_frh - m_fh;

                            if (diffW < diffH)
                            {
                                m_scw = m_frw;
                                m_sch = static_cast<int>(0.5f + static_cast<float>(m_fh*m_frw / m_fw));
                                if (m_sch % 2) --m_sch;
                                m_yOff = (m_frh - m_sch) >> 1;
                            }
                            else
                            {
                                m_sch = m_frh;
                                m_scw = static_cast<int>(0.5f + static_cast<float>(m_fw*m_frh / m_fh));
                                if (m_scw % 2) --m_scw;
                                m_xOff = (m_frw - m_scw) >> 1;
                            }
                        }
                    }
                    m_videoScalerContext = sws_getContext(m_fw, m_fh, PIX_FMT_RGB24,
                        m_scw, m_sch, PIX_FMT_BGRA, SWS_FAST_BILINEAR, 0, 0, 0);
                    m_tempBlock.resize(m_scw*m_sch * 4);
                    if (m_fps != 0) m_frameDelay = 1000 / m_fps;
                    else m_frameDelay = 60;
                    m_xOff = m_xOff * 4;
                    
                }
                unsigned char* rgbData = this->GetRawRGBData();
                uint8_t* rgb24Data[1] = { rgbData };
                uint8_t* bgraData[1] = { m_tempBlock.data() };
                static int linesizeRGB24[1] = { 3 * m_fw };
                static int linesizeBGRA[1] = { 4 * m_scw };
                sws_scale(m_videoScalerContext, rgb24Data, linesizeRGB24, 0, m_fh, bgraData, linesizeBGRA);
                uint8_t* bgra = m_rawBGRA.data();

                // maybe optimize the following code
                static int off1 = 0, off2 = 0;
                for (int y = 0; y < m_sch; y++)
                {
                    off1 = (((m_yOff + y)*m_frw) << 2) + m_xOff;
                    off2 = (y*m_scw) << 2;
                    for (int x = 0, lim = m_scw*4; x < lim; x+=4)
                    {
                        bgra[off1 + x + 0] = bgraData[0][off2 + x + 0];
                        bgra[off1 + x + 1] = bgraData[0][off2 + x + 1];
                        bgra[off1 + x + 2] = bgraData[0][off2 + x + 2];
                        bgra[off1 + x + 3] = bgraData[0][off2 + x + 3];
                    }
                }
                m_frameRenderer->SetBGRAData(bgra);
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
    }
    void StartPlaybackAsync()
    {
        if (m_frameRenderer) m_frameRenderer->Show();
        m_playbackThread = boost::thread(boost::bind(&VideoPlayback::StartPlayback, this));
    }
    void StopPlayback()
    {
        m_playbackStopped = true;
        if (m_frameRenderer) m_frameRenderer->Hide();
        m_playbackThread.join();
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
                // The packet was corrupted :/
            }
        }
    }
private:
    FrameRenderer *m_frameRenderer;
    SwsContext* m_videoScalerContext;
    GtkWidget* m_fixed;
    
    // position and size for frame renderer
    int m_frx, m_fry, m_frw, m_frh;
    int m_scw, m_sch;


    unsigned int m_frameDelay;
    boost::thread m_playbackThread;
    bool m_playbackStopped;
    bool m_newFrameAvailable;
    std::vector<uint8_t> m_rawBGRA;
    std::vector<uint8_t> m_tempBlock;
    int m_xOff, m_yOff;
};
