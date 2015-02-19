#pragma once

#include "common/common.h"
#include "client/MediaStream/VideoStream.h"
#include "client/UI/FrameRenderer.h"

class VideoPlayback :public VideoStream
{
public:
    VideoPlayback()
    {
        this->InitializeDecoder();
        m_newFrameAvailable = false;
        m_scaler = 0;
    }
    void Set(GtkWidget* fixed, int x, int y, int w, int h)
    {
        m_x = x; m_y = y;
        m_w = w; m_h = h;
        m_frameRenderer = new FrameRenderer;
        m_frameRenderer->Set(fixed, m_x, m_y, m_w, m_h);
        m_buffer.resize(4 * m_w*m_h);
    }
    void StartPlayback()
    {
        m_frameRenderer->Show();
        m_playbackStopped = false;
        while (!m_playbackStopped)
        {
            unsigned char* bgraData = this->GetRawBGRAData();
            m_newFrameAvailable= false;
            if (bgraData)
            {
                
                uint8_t * inData[1] = { bgraData };
                int inLinesize[1] = { 4 * m_fw };
                uint8_t* outData[1] = { m_buffer.data() };
                int outLinesize[1] = { 4*m_w };
                if(sws_scale(m_scaler, inData, inLinesize, 0, m_h, outData, outLinesize) > 0)
                    m_frameRenderer->SetBGRAData(m_buffer.data());
            }
            while (!m_newFrameAvailable)
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            }
        }
    }
    void SetPacket(AVPacket* pkt)
    {
        try
        {
            if (this->DecodeToFrame(pkt))
            {
                if (m_scaler == 0 && m_fw != 0)
                {
                    m_scaler = sws_getContext(m_fw, m_fh, AV_PIX_FMT_BGRA, m_w, m_h, AV_PIX_FMT_BGRA, SWS_BICUBIC, 0, 0, 0);
                }
                m_newFrameAvailable = true;
            }
        }
        catch (FailedToDecode)
        {

        }
    }
    void StartPlaybackAsync()
    {
        m_playbackThread = boost::thread(boost::bind(&VideoPlayback::StartPlayback, this));
    }
    void StopPlayback()
    {

    }
private:
    FrameRenderer *m_frameRenderer;
    bool m_newFrameAvailable;
    boost::thread m_playbackThread;
    bool m_playbackStopped;
    int m_x, m_y, m_w, m_h;
    SwsContext* m_scaler;
    std::vector<uint8_t> m_buffer;
};


//@@@@@@@@@@@@@@@@@@@@@@@@
// Temporary class
// just for test
class RandomVideoGenerator:public VideoStream
{
public:
    RandomVideoGenerator()
    {
        srand(time(0));
        this->InitializeEncoder(320, 240, 10, 1000000);
        running = true;
        boost::thread videoGeneratorThread([this](){
            const int w = 320, h = 240;
            uint8_t* buff = new uint8_t[w*h * 3];
            int pts = 0;
            int val = g_random_int() % 256;
            int dv = 1;
            while (running)
            {
                if (val >= 255 || val <= 0)
                    dv = -dv;
                val += dv;
                for (int y = 0; y < h; y++)
                {
                    for (int x = 0; x < w; x++)
                    {
                        buff[3 * (y*w + x) + 0] = val;
                        buff[3 * (y*w + x) + 1] = val;
                        buff[3 * (y*w + x) + 2] = val;
                    }
                }
                this->EncodeToPacket(buff, pts++);
                boost::this_thread::sleep(boost::posix_time::milliseconds(80));
            }
        });
    }
    void End()
    {
        running = false;
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
private:
    bool running;
};



class VideoPlaybackManager
{
public:
    void Set(GtkWidget* fixed, int x = 180, int y = 50, int w = 650, int h = 490)
    {
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;
        m_fixed = fixed;
        int fw = 320, fh = 240;
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 2; i++)
            {
                m_videoPlayers.push_back(new VideoPlayback);
                m_videoPlayers[m_videoPlayers.size() - 1]->Set(m_fixed, m_x + i*(fw + 10), m_y + j*(fh + 10), fw, fh);
                m_videoPlayers[m_videoPlayers.size() - 1]->StartPlaybackAsync();
            }
        }
    }
    // playerID = 0 to 3 for now
    void SetPacket(int playerID, AVPacket* pkt)
    {
        m_videoPlayers[playerID]->SetPacket(pkt);
    }
    void AddVideoPlaybackSource(int noOfVideoSources = 1)
    {
        
    }
    VideoPlaybackManager()
    {  
        
    }

private:
    std::vector<VideoPlayback*> m_videoPlayers;
    GtkWidget* m_fixed;
    int m_x, m_y, m_w, m_h;
};
