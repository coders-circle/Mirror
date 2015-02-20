#include "client/MediaStream/VideoPlayback.h"

VideoPlayback::VideoPlayback()
{
    this->InitializeDecoder();
    m_newFrameAvailable = false;
    m_scaler = 0;
}
void VideoPlayback::Set(GtkWidget* fixed, int x, int y, int w, int h)
{
    m_x = x; m_y = y;
    m_w = w; m_h = h;
    m_frameRenderer = new FrameRenderer;
    m_frameRenderer->Set(fixed, m_x, m_y, m_w, m_h);
    m_buffer.resize(4 * m_w*m_h);
}
void VideoPlayback::StartPlayback()
{
    m_frameRenderer->Show();
    m_playbackStopped = false;
    while (!m_playbackStopped)
    {
        unsigned char* bgraData = this->GetRawBGRAData();
        m_newFrameAvailable = false;
        if (bgraData)
        {

            uint8_t * inData[1] = { bgraData };
            int inLinesize[1] = { 4 * m_fw };
            uint8_t* outData[1] = { m_buffer.data() };
            int outLinesize[1] = { 4 * m_w };
            if (sws_scale(m_scaler, inData, inLinesize, 0, m_fh, outData, outLinesize) > 0)
                m_frameRenderer->SetBGRAData(m_buffer.data());
        }
        while (!m_newFrameAvailable && !m_playbackStopped)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
    }
}
void VideoPlayback::SetPacket(AVPacket* pkt)
{
    try
    {
        if (this->DecodeToFrame(pkt))
        {
            if (m_scaler == 0 && m_fw != 0)
            {
                m_scaler = sws_getContext(m_fw, m_fh, AV_PIX_FMT_BGRA, m_w, m_h, AV_PIX_FMT_BGRA, SWS_FAST_BILINEAR, 0, 0, 0);
            }
            m_newFrameAvailable = true;
        }
    }
    catch (FailedToDecode)
    {

    }
}

void VideoPlayback::SetPacket(const uint8_t* data, size_t size)
{
    try
    {
        if (this->DecodeToFrame(data, size))
        {
            if (m_scaler == 0 && m_fw != 0)
            {
                m_scaler = sws_getContext(m_fw, m_fh, AV_PIX_FMT_BGRA, m_w, m_h, AV_PIX_FMT_BGRA, SWS_FAST_BILINEAR, 0, 0, 0);
            }
            m_newFrameAvailable = true;
        }
    }
    catch (FailedToDecode)
    {

    }

}
void VideoPlayback::StartPlaybackAsync()
{
    m_playbackThread = boost::thread(boost::bind(&VideoPlayback::StartPlayback, this));
}
void VideoPlayback::StopPlayback()
{
    m_playbackStopped = true;
    if (m_playbackThread.joinable()) m_playbackThread.join();
}


void VideoPlaybackManager::Set(GtkWidget* fixed, int x, int y, int w, int h)
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
void VideoPlaybackManager::SetPacket(int playerID, AVPacket* pkt)
{
    m_videoPlayers[playerID]->SetPacket(pkt);
}

void VideoPlaybackManager::SetPacket(int playerID, const uint8_t* data, size_t size)
{
    m_videoPlayers[playerID]->SetPacket(data, size);
}

VideoPlaybackManager::VideoPlaybackManager()
{
}

void VideoPlaybackManager::AddPlayback(int id)
{
}

void VideoPlaybackManager::Stop()
{
    for (auto videoPlayer : m_videoPlayers)
        videoPlayer->StopPlayback();
}