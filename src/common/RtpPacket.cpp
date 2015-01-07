#include <common/common.h>
#include <common/RtpPacket.h>

RtpPacket::RtpPacket()
: m_udpHandler(NULL)
{
}

RtpPacket::~RtpPacket()
{
    CleanUp();
}

void RtpPacket::Initialize(UdpHandler* udpHandler, const udp::endpoint &remoteEndpoint)
{
    m_udpHandler = udpHandler;
    m_remoteEndpoint = remoteEndpoint;
    m_sequenceNumber = m_timeStamp = 0;
    m_timeStampIncrement = 3600;        // for 25fps video
}

void RtpPacket::CleanUp()
{
    m_remoteEndpoint = udp::endpoint();
    m_udpHandler = NULL;
}

void RtpPacket::Send(const uint8_t *data, size_t size)
{
    if (!m_udpHandler)
        throw RtpTransmissionException("RTP packet hasn't been properly initialized");

    std::vector<uint8_t> packet;
    packet.resize(RTP_HEADER_SIZE + size);
    packet[0] = (uint8_t)(m_marker?0x81:0x80);                               // RTP version
    packet[1] = m_payloadType;                      // Payload type
    packet[2] = m_sequenceNumber >> 8;              // Sequence number of packet
    packet[3] = m_sequenceNumber & 0x0FF;           
    packet[4] = (m_timeStamp & 0xFF000000) >> 24;   // Timestamp
    packet[5] = (m_timeStamp & 0x00FF0000) >> 16;
    packet[6] = (m_timeStamp & 0x0000FF00) >> 8;
    packet[7] = (m_timeStamp & 0x000000FF);
    packet[8] = (uint8_t)0x13;                               // 4 byte SSRC (sychronization source identifier)
    packet[9] = (uint8_t)0xf9;                               // we just an arbitrary number here to keep it simple
    packet[10] = (uint8_t)0x7e;
    packet[11] = (uint8_t)0x67;

    memcpy(&packet[RTP_HEADER_SIZE], data, size);
    /*for (unsigned int i = 0; i < size; ++i)
        packet[RTP_HEADER_SIZE + i] = data[i];*/

    m_sequenceNumber++;
    m_timeStamp += m_timeStampIncrement;

    m_udpHandler->Send(m_remoteEndpoint, (char*)&packet[0], packet.size());
}


size_t RtpPacket::Receive(uint8_t* data, size_t maxSize)
{
    if (!m_udpHandler)
        throw RtpReceptionError("RTP packet hasn't been properly initialized");

    std::vector<uint8_t> packet;
    packet.resize(RTP_HEADER_SIZE + maxSize);
    size_t len = m_udpHandler->Receive(m_remoteEndpoint, (char*)&packet[0], packet.size());
    if (len == RTP_HEADER_SIZE) return 0;
    //packet.resize(len);

    uint8_t version = packet[0];
    if (version & 0x01 == 0x01) m_marker = true;
    else m_marker = false;
    m_payloadType = packet[1];
    m_sequenceNumber = *((uint16_t*)&packet[2]);
    m_timeStamp = *((int*)&packet[4]);
    int SSRC = *((int*)&packet[8]);

    memcpy(data, &packet[RTP_HEADER_SIZE], len - RTP_HEADER_SIZE);
    /*for (unsigned int i = 0; i < len - RTP_HEADER_SIZE; ++i)
        data[i] = packet[i + RTP_HEADER_SIZE];*/
    return len - RTP_HEADER_SIZE;
}
