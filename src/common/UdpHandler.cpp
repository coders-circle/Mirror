#include <common/common.h>
#include <common/UdpHandler.h>

UdpHandler::UdpHandler(boost::asio::io_service &ioService)
: m_ioService(ioService)
{}

UdpHandler::~UdpHandler()
{}


void UdpHandler::Initialize(boost::shared_ptr<udp::socket> socket)
{
    if (m_socket)
        throw UdpHandlerException("Socket alread created");
    m_socket = socket;
}

void UdpHandler::Initialize(const udp::endpoint &localEndpoint)
{
    if (m_socket)
        throw UdpHandlerException("Socket alread created");
    m_socket.reset(new udp::socket(m_ioService, localEndpoint));
}

void UdpHandler::Send(const udp::endpoint &remoteEndpoint, const char* data, size_t size)
{
    if (m_socket)
        throw UdpHandlerException("Socket not created");
    m_socket->send_to(boost::asio::buffer(data, size), remoteEndpoint);
}

size_t UdpHandler::Receive(udp::endpoint &remoteEndpoint, char* data, size_t maxSize)
{
    if (m_socket)
        throw UdpHandlerException("Socket not created");
    return m_socket->receive_from(boost::asio::buffer(data, maxSize), remoteEndpoint);
}