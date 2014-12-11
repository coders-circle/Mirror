#include <common/common.h>
#include <client/TcpClient.h>

TcpClient::TcpClient(boost::asio::io_service &io)
:m_io(io)
{}

TcpClient::~TcpClient()
{}

