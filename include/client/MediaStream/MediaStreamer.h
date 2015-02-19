#pragma once
#include "RtpStreamer.h"

enum MediaTypes { MEDIA_VIDEO, MEDIA_AUDIO };
class MediaStreamer
{
public:
    MediaStreamer() : m_rtpStreamer(NULL), m_streaming(NULL) {}

    // This function is automatically called by the Client object
    // Donot call this directly
    // Instead use Client::StartStreaming(MediaStreamer&) method
    void InitializeStreamer(RtpStreamer* rtpStreamer, bool* streaming, UdpHandler* udpHandler, udp::endpoint &endpoint, uint32_t clientId)
    {
        m_rtpStreamer = rtpStreamer;
        m_streaming = streaming;
        m_rtpPacket.Initialize(udpHandler, endpoint);
        m_rtpPacket.SetSourceId(clientId);
    }
    
    // This function is automatically called by the Client object
    void Receive(uint32_t sourceId, uint8_t mediaType, const uint8_t* data, size_t len)
    {
        if ((int)mediaType == MEDIA_VIDEO)
        {
            if (m_videoCallback)
                m_videoCallback(sourceId, data, len);
        }
        else if ((int)mediaType == MEDIA_AUDIO)
        {
            if (m_audioCallback)
                m_audioCallback(sourceId, data, len);
        }
    }

    // Send some media data to the server
    void Send(uint8_t mediaType, const uint8_t* data, size_t len)
    {
        if (m_streaming && m_rtpStreamer && *m_streaming)
        {
            m_rtpPacket.SetPayloadType(mediaType);
            m_rtpStreamer->Send(m_rtpPacket, data, len);
        }
    }
    
    // Set callback for handling received media
    void SetVideoHandler(std::function<void(uint32_t sourceId, const uint8_t* data, size_t len)> f) { m_videoCallback = f; }
    void SetAudioHandler(std::function<void(uint32_t sourceId, const uint8_t* data, size_t len)> f) { m_audioCallback = f; }

private:
    std::function<void(uint32_t sourceId, const uint8_t* data, size_t len)> m_videoCallback;
    std::function<void(uint32_t sourceId, const uint8_t* data, size_t len)> m_audioCallback;

    RtpStreamer* m_rtpStreamer;
    bool* m_streaming;
    RtpPacket m_rtpPacket;
};
