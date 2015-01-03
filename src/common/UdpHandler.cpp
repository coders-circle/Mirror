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

void UdpHandler::Close()
{
    if (m_socket)
        m_socket->close();
    m_socket.reset();
}

void UdpHandler::GetUdpPairs(UdpHandler &handler1, UdpHandler &handler2)
{
    bool even, odd=false;
    while (!odd)
    {
        even = false;
        while (!even)
        {
            try
            {
                handler1.Initialize(udp::endpoint(udp::v4(), 0));
                if (handler1.GetSocket()->local_endpoint().port()%2==0)
                    even=true;
                else
                    handler1.Close();
            }
            catch(...) {}
        }
        try
        {
            uint16_t port1 = handler1.GetSocket()->local_endpoint().port();
            handler2.Initialize(udp::endpoint(udp::v4(), port1+1));
            if (handler2.GetSocket()->local_endpoint().port()==port1+1)
                odd=true;
            else
            {
                handler1.Close();
                handler2.Close();
            }
        }
        catch(...) { handler1.Close(); }
    }
}
