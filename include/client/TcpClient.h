#pragma once
#include <common/TcpListener.h>
#include <common/TcpHandler.h>
#include <common/RequestHandler.h>

class TcpClient
{
public:
    TcpClient(boost::asio::io_service &io);
    ~TcpClient();

    // Start listening for incoming connections in a new thread
    void StartListening(const tcp::endpoint& localEndPoint);
    // Connect to a peer
    void Connect(const tcp::endpoint& peer);
    
    // Test Method
    void StartChatSession();
    void JoinGroup(uint32_t groupId);

private:
    boost::asio::io_service &m_io;
    TcpListener m_listener;
    TcpHandler m_tcpHandler;
    RequestHandler m_requests;

    void ListenerHandler(boost::shared_ptr<tcp::socket> socket);
};