#pragma once
class TcpClient
{
public:
    TcpClient(boost::asio::io_service &io);
    ~TcpClient();

    void StartListening();

private:
    boost::asio::io_service &m_io;
};