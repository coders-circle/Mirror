#include "common/common.h"
#include "common/TcpHandler.h"

TcpHandler::TcpHandler(boost::asio::io_service &ioService) : 
	m_ioService(ioService)
{
}

TcpHandler::~TcpHandler()
{
}

// this is initialized by the Listener which accepts 
//  a connection from a peer and creates a socket
void TcpHandler::Initialize(boost::shared_ptr<tcp::socket> socket)
{
    if (m_socket)
        throw TcpHandlerException("Socket alread created");
    m_socket = socket;
    std::cout << "Connected to " << m_socket->remote_endpoint().address().to_string() << "  " << m_socket->remote_endpoint().port() << std::endl;
}

// this is used to create a new socket to connect to a peer at
//  given endpoint
void TcpHandler::Initialize(const tcp::endpoint &destEndpoint)
{
    if (m_socket)
        throw TcpHandlerException("Socket alread created");
	m_socket.reset(new tcp::socket(m_ioService));
    m_socket->connect(destEndpoint);
    std::cout << "Connected to " << m_socket->remote_endpoint().address().to_string() << "  " << m_socket->remote_endpoint().port() << std::endl;
}

// 
void TcpHandler::Send(const char* data, size_t size)
{
    if (!m_socket) return;
	boost::asio::write(*m_socket, boost::asio::buffer(data, size));
}

//
void TcpHandler::Receive(char* data, size_t max_size)
{
    if (!m_socket) return; 
    m_socket->read_some(boost::asio::buffer(data, max_size));;
}
