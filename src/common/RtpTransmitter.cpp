#include <common/common.h>
#include <common/RtpTransmitter.h>

#define RTP_HEADER_SIZE 12 

RtpTransmitter::RtpTransmitter()
{
    CleanUp();
}

RtpTransmitter::~RtpTransmitter()
{}

void RtpTransmitter::Initialize(boost::shared_ptr<boost::asio::ip::udp::socket> socket, const boost::asio::ip::udp::endpoint &clientEndpoint)
{
    m_socket = socket;
    m_clientEndpoint = clientEndpoint;
    m_sequenceNumber = m_timeStamp = 0;
    m_timeStampIncrement = 3600;        // for 25fps video
}

void RtpTransmitter::CleanUp()
{
    m_socket.reset();
    m_clientEndpoint = boost::asio::ip::udp::endpoint();
}

void RtpTransmitter::Send(const std::vector<char> &data)
{
    if (!m_socket)
        throw RtpTransmissionException("RTP Transmitter hasn't been properly initialized");

    std::vector<char> packet;
    packet.resize(RTP_HEADER_SIZE);
    packet[0] = 0x80;                               // RTP version
    packet[1] = m_payloadType;                      // Payload type
    packet[2] = m_sequenceNumber >> 8;              // Sequence number of packet
    packet[3] = m_sequenceNumber & 0x0FF;           
    packet[4] = (m_timeStamp & 0xFF000000) >> 24;   // Timestamp
    packet[5] = (m_timeStamp & 0x00FF0000) >> 16;
    packet[6] = (m_timeStamp & 0x0000FF00) >> 8;
    packet[7] = (m_timeStamp & 0x000000FF);
    packet[8] = 0x13;                               // 4 byte SSRC (sychronization source identifier)
    packet[9] = 0xf9;                               // we just an arbitrary number here to keep it simple
    packet[10] = 0x7e;
    packet[11] = 0x67;

    packet.insert(packet.end(), data.begin(), data.end());

    m_sequenceNumber++;
    m_timeStamp += m_timeStampIncrement;

    m_socket->send_to(boost::asio::buffer(packet), m_clientEndpoint);
}