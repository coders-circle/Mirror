#include <common/common.h>
#include <common/RtpHandler.h>

RtpHandler::RtpHandler()
{
}

RtpHandler::~RtpHandler()
{
    CleanUp();
}

void RtpHandler::Initialize(boost::shared_ptr<udp::socket> socket, const udp::endpoint &remoteEndpoint)
{
    m_socket = socket;
    m_remoteEndpoint = remoteEndpoint;
    m_sequenceNumber = m_timeStamp = 0;
    m_timeStampIncrement = 3600;        // for 25fps video
}

void RtpHandler::CleanUp()
{
    m_socket.reset();
    m_remoteEndpoint = udp::endpoint();
}

void RtpHandler::Send(const char *data, size_t size)
{
    if (!m_socket)
        throw RtpTransmissionException("RTP Transmitter hasn't been properly initialized");

    std::vector<char> packet;
    packet.resize(RTP_HEADER_SIZE);
    packet[0] = (char)0x80;                               // RTP version
    packet[1] = m_payloadType;                      // Payload type
    packet[2] = m_sequenceNumber >> 8;              // Sequence number of packet
    packet[3] = m_sequenceNumber & 0x0FF;           
    packet[4] = (m_timeStamp & 0xFF000000) >> 24;   // Timestamp
    packet[5] = (m_timeStamp & 0x00FF0000) >> 16;
    packet[6] = (m_timeStamp & 0x0000FF00) >> 8;
    packet[7] = (m_timeStamp & 0x000000FF);
    packet[8] = (char)0x13;                               // 4 byte SSRC (sychronization source identifier)
    packet[9] = (char)0xf9;                               // we just an arbitrary number here to keep it simple
    packet[10] = (char)0x7e;
    packet[11] = (char)0x67;

    for (unsigned int i = 0; i < size; ++i)
        packet[RTP_HEADER_SIZE + i] = data[i];

    m_sequenceNumber++;
    m_timeStamp += m_timeStampIncrement;

    m_socket->send_to(boost::asio::buffer(packet), m_remoteEndpoint);
}


void RtpHandler::Receive(char* data, size_t maxSize)
{
    if (!m_socket)
        throw RtpReceptionError("RTP Receiver hasn't been properly initialized");

    std::vector<char> packet;
    packet.resize(maxSize);
    size_t len = m_socket->receive_from(boost::asio::buffer(packet), m_remoteEndpoint);
    packet.resize(len);

    char version = packet[0];
    m_payloadType = packet[1];
    m_sequenceNumber = *((uint16_t*)&packet[2]);
    m_timeStamp = *((int*)&packet[4]);
    int SSRC = *((int*)&packet[8]);

    for (unsigned int i = 0; i < len - RTP_HEADER_SIZE; ++i)
        data[i] = packet[i + RTP_HEADER_SIZE];
}