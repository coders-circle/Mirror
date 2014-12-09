#include <common/common.h>
#include <common/RtpSreamer.h>


RtpStreamer::RtpStreamer()
{
    CleanUp();
}

RtpStreamer::~RtpStreamer()
{}

void RtpStreamer::Initialize(boost::shared_ptr<boost::asio::ip::udp::socket> socket, const boost::asio::ip::address &clientAddress)
{
    m_socket = socket;
    m_clientAddress = clientAddress;
}

void RtpStreamer::CleanUp()
{
    m_socket.reset();
    m_clientAddress = boost::asio::ip::address();
}