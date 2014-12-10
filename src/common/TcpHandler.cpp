#include <common/common.h>
#include <common/TcpHandler.h>

TcpHandler::TcpHandler() : 
{
}

TcpHandler::~TcpHandler()
{
}

TcpHandler::Initialize(boost::shared_ptr<tcp::socket> socket)
{
	m_socket = socket;
}

TcpHandler::Request(tcp::endpoint & destEndpoint)
{
	// since the host can have multiple addresses, we iterate
	//  through the endpoints

	// if the connect fails, and the socket was already opened,
	//   the socket is not returned to the closed state
	
	boost::system::error_code error;
	socket->connect(destEndpoint, error);

