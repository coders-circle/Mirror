#pragma once
#include <common/TcpHandler.h>
#include <common/TcpRequest.h>

/* Data passed to the message event handler */
struct MessageEventData
{
    size_t connectionId;
    std::string senderId;
    std::string message;
};

/* Class to handle networking connections in client application */
class Client
{
public:
    Client();
    ~Client();

    // Boost IO service
    boost::asio::io_service& GetIOService() { return m_io; }

    // Connect to a peer/server,
    size_t Connect(const tcp::endpoint& peer);
    // Connect to a peer/server asynchronously
    void ConnectAsync(const tcp::endpoint& peer, bool* threadEnd = NULL, size_t* connectionId = NULL);
    // Connect to a peer through server
    size_t Connect(uint32_t clientId);
    // Connect to a peer through server asynchronously
    void ConnectAsync(uint32_t clientId, bool* threadEnd = NULL, size_t* connectionId = NULL);
    // Start handling all requests that the client gets
    void HandleRequests();
    // Start handling all requests asynchronously
    void HandleRequestsAsync();
    // Disconnect
    void Disconnect(size_t connectionId);
    // Check if connected
    bool IsConnected(size_t connectionId);

    // Set the event handler to handle incoming chat messages
    void SetMessageEventHandler(std::function<void(boost::shared_ptr<MessageEventData>)> handler) { m_messageHandler = handler; }
    // Send chat message to a connection (receiverId = 0 for server)
    void SendMessage(size_t receiverId, const std::string& message, uint32_t groupId = 0);
    // Event handler to handle a join chat request from a peer (for p2p only)
    void SetJoinChatEventHandler(std::function<bool(size_t)> handler) { m_joinChatHandler = handler; }

    // Send a join chat request; on successful returns true
    bool JoinChat(uint32_t connectionId, uint32_t groupId = 0);
    // Set username to use while sending messages
    void SetName(const std::string &name) { m_name = name; }
    const std::string& GetName() { return m_name; }

private:
    boost::asio::io_service m_io;
    // Event handlers
    std::function<void(boost::shared_ptr<MessageEventData>)> m_messageHandler;
    std::function<bool(size_t)> m_joinChatHandler;

    // Connection representing a tcp-connection with a peer/server
    struct Connection
    {
        Connection(boost::asio::io_service &io)
        : tcpHandler(io)
        {}
        TcpHandler tcpHandler;
        bool connected;
        // maybe store userid and other stuffs here...
    };

    // List of the connections
    std::vector<Connection> m_connections;
    // Mutex to lock the use of common variables (m_connections/m_request/...) during multithreading
    boost::mutex m_mutex;

    TcpRequest m_request;
    std::string m_name;

    // For P2P:
    bool m_p2pConnecting;
    size_t HandleP2PRequest(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint);
    void HandleP2PRequestAsync(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint);
    boost::shared_ptr<tcp::acceptor> m_acceptor;
    void P2PListen(const tcp::endpoint &localEndpoint);
    void P2PConnect(tcp::endpoint &remoteEndpoint);

};
