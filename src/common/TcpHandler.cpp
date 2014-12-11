#include <common/common.h>
#include <common/TcpHandler.h>

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
	m_socket = socket;
}

// 
void TcpHandler::Initialize(tcp::endpoint &destEndpoint)
{
	m_socket.reset(new tcp::socket(m_ioService));
}

// 
void TcpHandler::Send(const std::vector<char> &data)
{
	boost::asio::write(*m_socket, boost::asio::buffer(data));
	m_lastSent = data;
}

// 
void TcpHandler::Receive(std::vector<char> &data)
{
	boost::system::error_code error;
	size_t len = m_socket->read_some(boost::asio::buffer(data), error);

	if (error == boost::asio::error::eof) // connection closed by the peer
	{
		//
	}
	else if (error)
		throw TcpHandlerException("Error receiving data : " + error.message());

	// do something with the received data
}

void TcpHandler::Request(tcp::endpoint &destEndpoint)
{
	m_socket->connect(destEndpoint);
	m_destination.reset(new tcp::endpoint(destEndpoint));
}
