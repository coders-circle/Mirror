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
	m_socket = boost::shared_ptr<tcp::socket>(new tcp::socket(m_ioService);
}

// 
void Send(const std::vector<char> &data)
{
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(data),
			boost::asio::transfer_all(), error);
	if (error)
		throw TcpHandlerException("Error sending data : " + error.what());
	m_lastSent = data;
}

// 
void Receive(const std::vector<char> &data)
{
	boost::system::error_code error;
	size_t len = socket.read_some(boost::asio::buffer(data), error);

	if (error == boost::asio::error::eof) // connection closed by the peer
	{
		//
	}
	else if (error)
		throw TcpHandlerException("Error receiving data : " + error.what());

	// do something with the received data
}

void TcpHandler::Request(tcp::endpoint &destEndpoint)
{
	// since the host can have multiple addresses, we iterate
	//  through the endpoints

	// if the connect fails, and the socket was already opened,
	//   the socket is not returned to the closed state
	
	boost::system::error_code error;
	m_socket->connect(destEndpoint, error);
	
	if (error)
		throw TcpHandlerException("Error connecting to peer : " + error.what());
	// assign the destEndpoint to m_destination
	m_destination = new tcp::endpoint(destEndpoint);
}
