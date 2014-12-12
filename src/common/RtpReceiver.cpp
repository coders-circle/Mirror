#include <common/common.h>
#include <common/RtpReceiver.h>

RtpReceiver::RtpReceiver()
{}

RtpReceiver::~RtpReceiver()
{
    CleanUp();
}

void RtpReceiver::Initialize(boost::shared_ptr<udp::socket> socket)
{
    m_socket = socket;
    m_sequenceNumber = m_timeStamp = 0;
}

void RtpReceiver::CleanUp()
{
    m_socket.reset();
}

void RtpReceiver::Receive(char *data, size_t maxSize)
{
    if (!m_socket)
        throw RtpReceptionError("RTP Receiver hasn't been properly initialized");

    std::vector<char> packet;
    packet.resize(maxSize);
    size_t len = m_socket->receive_from(boost::asio::buffer(packet), m_source);
    packet.resize(len);

    char version = packet[0];
    m_payloadType = packet[1];
    m_sequenceNumber = *((uint16_t*)&packet[2]);
    m_timeStamp = *((int*)&packet[4]);
    int SSRC = *((int*)&packet[8]);
    
    for (unsigned int i = 0; i < len - RTP_HEADER_SIZE; ++i)
        data[i] = packet[i + RTP_HEADER_SIZE];
}