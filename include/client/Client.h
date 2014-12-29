#pragma once
#include <common/TcpHandler.h>
#include <common/TcpRequest.h>


class Client
{
public:
    Client();
    ~Client();

    // Boost IO service
    boost::asio::io_service& GetIOService() { return m_io; }

    // Connect to a peer/server,
    size_t Connect(const tcp::endpoint& peer, bool* successful = NULL);
    // Connect to a peer/server asynchronously
    void ConnectAsync(const tcp::endpoint& peer, bool* successful = NULL);
    // Connect to a peer through server
    size_t Connect(uint32_t clientId, bool* successful = NULL);
    // Start handling all requests that the client gets
    void HandleRequests();

    // Join chat, for p2p set groupId to 0
    void JoinChat(uint32_t connectionId, uint32_t groupId = 0);
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

    boost::asio::io_service m_io;

    // List of the connections
    std::vector<Connection> m_connections;
    // Mutex to lock the use of common variables (m_connections/m_request/...) during multithreading
    boost::mutex m_mutex;

    TcpRequest m_request;
    std::string m_name;

    // For P2P:
    bool m_p2pConnecting;
    size_t HandleP2PRequest(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint, bool* successful = NULL);
    void HandleP2PRequestAsync(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint, bool* successful = NULL);
    boost::shared_ptr<tcp::acceptor> m_acceptor;
    void P2PListen(const tcp::endpoint &localEndpoint);
    void P2PConnect(tcp::endpoint &remoteEndpoint);

    // For console:
    int m_currentConnection;
    // Separate thread for inputting chat messages
    void ChatInput(uint32_t groupId);
};