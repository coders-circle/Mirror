#include "common/common.h"
#include "common/TcpHandler.h"

TcpHandler::TcpHandler(boost::asio::io_service &ioService) : 
	m_ioService(ioService)
{
}

TcpHandler::~TcpHandler()
{
}

void TcpHandler::Close()
{
    if (m_socket)
        m_socket->close();
}

// this is initialized by the Listener which accepts 
//  a connection from a peer and creates a socket
void TcpHandler::Initialize(boost::shared_ptr<tcp::socket> socket)
{
    if (m_socket)
        throw TcpHandlerException("Socket alread created");
    m_socket = socket;
}

// this is used to create a new socket to connect to a peer at
//  given endpoint
void TcpHandler::Initialize(const tcp::endpoint &destEndpoint)
{
    if (m_socket)
        throw TcpHandlerException("Socket alread created");
    m_socket.reset(new tcp::socket(m_ioService));
    m_socket->connect(destEndpoint);
}

// 
void TcpHandler::Send(const char* data, size_t size)
{
    if (!m_socket)
        throw TcpHandlerException("Socket not created");
	boost::asio::write(*m_socket, boost::asio::buffer(data, size));
}

//
void TcpHandler::Receive(char* data, size_t size)
{
    if (!m_socket)
        throw TcpHandlerException("Socket not created");
    boost::asio::read(*m_socket, boost::asio::buffer(data, size));
}

size_t TcpHandler::ReceiveSome(char* data, size_t max_size)
{
    if (!m_socket)
        throw TcpHandlerException("Socket not created");
    return m_socket->read_some(boost::asio::buffer(data, max_size));
}

size_t TcpHandler::Available()
{
    if (!m_socket)
        throw TcpHandlerException("Socket not created");
    return m_socket->available();
}

std::string TcpHandler::GetDestinationAddress() const
{
    std::stringstream ss;
    ss << "IP: " << m_socket->remote_endpoint().address().to_string() << "  Port: " << m_socket->remote_endpoint().port();
    return ss.str();
}
