#pragma once

#include "common/common.h"
#include "client/MediaStream/VideoStream.h"
#include "client/UI/FrameRenderer.h"

class VideoPlayback :public VideoStream
{
public:
    VideoPlayback();
    void Set(GtkWidget* fixed, int x, int y, int w, int h);
    void StartPlayback();
    void SetPacket(AVPacket* pkt);
    void StartPlaybackAsync();
    void StopPlayback();
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
        srand(static_cast<unsigned int>(time(0)));
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

#include <map>


class VideoPlaybackManager
{
public:
    void Set(GtkWidget* fixed, int x = 180, int y = 50, int w = 650, int h = 490);
    void SetPacket(int playerID, AVPacket* pkt);
    VideoPlaybackManager();
    void AddPlayback(int id);
private:
    std::vector<VideoPlayback*> m_videoPlayers;
    GtkWidget* m_fixed;
    int m_x, m_y, m_w, m_h;
    std::map<int, int> m_idMap;
};
