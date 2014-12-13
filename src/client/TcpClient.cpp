#include <common/common.h>
#include <client/TcpClient.h>

TcpClient::TcpClient(boost::asio::io_service &io)
:m_io(io), m_listener(io)
{}

TcpClient::~TcpClient()
{}

void TcpClient::StartListening(const tcp::endpoint& server)
{
    m_listener.Initialize(server);
    m_listener.Listen(boost::bind(&TcpClient::ListenerHandler, this, _1));
}


void TcpClient::ListenerHandler(boost::shared_ptr<tcp::socket> socket)
{

}

