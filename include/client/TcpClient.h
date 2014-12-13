#pragma once
#include <common/TcpListener.h>
#include <common/TcpHandler.h>

class TcpClient
{
public:
    TcpClient(boost::asio::io_service &io);
    ~TcpClient();

    void StartListening(const tcp::endpoint& server);
    void Connect(const tcp::endpoint& server);
    
    // Test Method
    void StartChatSession();

    TcpHandler& GetHandler() { return m_handler; }
private:
    boost::asio::io_service &m_io;
    TcpListener m_listener;
    TcpHandler m_handler;

    void ListenerHandler(boost::shared_ptr<tcp::socket> socket);
};