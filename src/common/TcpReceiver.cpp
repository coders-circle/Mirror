#include <common/common.h>
#include <common/TcpReceiver.h>

TcpReceiver::TcpReceiver()
{
}

TcpReceiver::~TcpReceiver()
{
}

void TcpReceiver::Initialize(boost::shared_ptr<tcp::socket> socket, const tcp::endpoint &destination)
{
	m_socket = socket;
	m_destination = destination;
}

void TcpReceiver::Receive(std::vector<char> &data)
{
	if(!m_socket)
		throw TcpReceiverException("TCP Receiver hasn't been properly initialized");

	boost::system::error_code error;
	size_t len = m_socket->read_some(boost::asio::buffer(data), error);

	if (error == boost::asio::error::eof)
	{
		// Connection closed by the peer
		// do something
	}
	else if (error) // some other error
	{
		throw boost::system::system_error(error); 
	}
	// do something with the data
	// for now, just display the data
	std::cout.write(data.data(), len);
}

