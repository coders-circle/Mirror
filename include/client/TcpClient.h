#pragma once
#include <common/TcpAcceptor.h>
#include <common/TcpHandler.h>
#include <common/TcpRequest.h>


class TcpClient
{
public:
    TcpClient(boost::asio::io_service &io);
    ~TcpClient();

    // Connect to a peer/server
    void Connect(const tcp::endpoint& peer, bool joinChat = false);
    // Connect to a peer/server asynchronously
    void ConnectAsync(const tcp::endpoint& peer, bool joinChat = false);
    // Connect to a peer through server
    void Connect(uint32_t clientId);
    // Start handling all requests that the client gets
    void HandleRequests();

    // Join chat
    void JoinChat(uint32_t connectionId, uint32_t groupId);
    // Set username to use while sending messages
    void SetName(const std::string &name) { m_name = name; }

    // For console:
    // Start Chat Input in a separate thread
    void StartChatInput(uint32_t groupId);

private:
    // Connection representing a tcp-connection with a peer/server
    struct Connection
    {
        Connection(boost::asio::io_service &io)
        : tcpHandler(io)
        {}
        TcpHandler tcpHandler;
        // maybe store userid and other stuffs here...
    };

    boost::asio::io_service &m_io;

    // List of the connections
    std::vector<Connection> m_connections;
    // Mutex to lock 'm_connections' list as well as 'm_request' request-processor
    boost::mutex m_mutex;

    TcpRequest m_request;
    std::string m_name;

    // For P2P:
    bool m_p2pConnected;
    void HandleP2PRequest(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint);
    void HandleP2PRequestAsync(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint);
    boost::shared_ptr<tcp::acceptor> m_acceptor;
    void P2PListen(const tcp::endpoint &localEndpoint);
    void P2PConnect(tcp::endpoint &remoteEndpoint);

    // For console:
    int m_currentConnection;
    // Separate thread for inputting chat messages
    void ChatInput(uint32_t groupId);
};