#pragma once
#include <common/TcpListener.h>

class TcpClient
{
public:
    TcpClient(boost::asio::io_service &io);
    ~TcpClient();

    void StartListening(const tcp::endpoint& server);

private:
    boost::asio::io_service &m_io;
    TcpListener m_listener;

    void ListenerHandler(boost::shared_ptr<tcp::socket> socket);
};