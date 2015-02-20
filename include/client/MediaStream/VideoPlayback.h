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
    void SetPacket(const uint8_t* data, size_t size);
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

#include <map>


class VideoPlaybackManager
{
public:
    void Set(GtkWidget* fixed, int x = 180, int y = 50, int w = 650, int h = 490);
    void SetPacket(int playerID, AVPacket* pkt);
    void SetPacket(int playerID, const uint8_t* data, size_t size);
    VideoPlaybackManager();
    void AddPlayback(int id);
    void Stop();
private:
    std::vector<VideoPlayback*> m_videoPlayers;
    GtkWidget* m_fixed;
    int m_x, m_y, m_w, m_h;
    std::map<int, int> m_idMap;
};
