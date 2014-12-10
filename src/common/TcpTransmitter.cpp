#include <common/common.h>
#include <common/TcpTransmitter.h>

TcpTransmitter::TcpTransmitter()
{
}

TcpTransmitter::~TcpTransmitter()
{
}


void TcpTransmitter::Initialize(boost::shared_ptr<tcp::socket> socket, const udp::endpoint &destination)
{
	m_socket = socket;
	m_destination = destination;
}

void TcpTransmitter::Send(const std::vector<char> &data)
{
	if(!m_socket)
		throw TcpTransmitterException("TCP Transmitter hasn't been properly initialized");

	m_socket->send(boost::asio::buffer(data, data.size()));
}
