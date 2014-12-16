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
    void StartListening(/*const tcp::endpoint& localEndPoint*/);
    // Connect to a peer (server/client)
    void Connect(const tcp::endpoint& peer);
    
    // Test Methods
    // Join a group 
    void JoinGroup(uint32_t groupId, const std::string &name);
    // Start a group chat
    void StartChatSession(uint32_t groupId);

private:
    boost::asio::io_service &m_io;
    TcpListener m_listener;
    TcpHandler m_tcpHandler;
    RequestHandler m_requests;
    std::string m_name;

    void ListenerHandler(boost::shared_ptr<tcp::socket> socket);

    // Group chat session to receive chat messages
    void ChatSession(uint32_t groupId);
    // For console, we use separate thread for inputting chat messages
    void ChatInput(uint32_t groupId);
};