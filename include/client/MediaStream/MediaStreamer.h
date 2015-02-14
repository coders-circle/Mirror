#pragma once
#include "RtpStreamer.h"

class MediaStreamer
{
public:
    MediaStreamer() : m_rtpStreamer(NULL), m_streaming(NULL) {}
    void InitializeStreamer(RtpStreamer* rtpStreamer, bool* streaming, UdpHandler* udpHandler, udp::endpoint &endpoint, uint32_t clientId)
    {
        m_rtpStreamer = rtpStreamer;
        m_streaming = streaming;
        m_rtpPacket.Initialize(udpHandler, endpoint);
        m_rtpPacket.SetSourceId(clientId);
    }

    void Receive(uint32_t sourceId, const uint8_t* data, size_t len)
    {
        std::cout << "Received data: Length=" << len << " from source #" << sourceId << std::endl;
        if (m_receiveCallback)
            m_receiveCallback(sourceId, data, len);
    }

    void Send(const uint8_t* data, size_t len)
    {
        if (m_streaming && m_rtpStreamer && *m_streaming)
        {
            m_rtpStreamer->Send(m_rtpPacket, data, len);
        }
    }

    void SetReceiveHandler(std::function<void(uint32_t sourceId, const uint8_t* data, size_t len)> f) { m_receiveCallback = f; }

private:
    std::function<void(uint32_t sourceId, const uint8_t* data, size_t len)> m_receiveCallback;

    RtpStreamer* m_rtpStreamer;
    bool* m_streaming;
    RtpPacket m_rtpPacket;
};
