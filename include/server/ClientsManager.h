#pragma once
#include <common/TcpListener.h>
#include <common/TcpHandler.h>
#include <common/RequestHandler.h>

struct ClientInfo
{
    ClientInfo(boost::asio::io_service& io) : connection(io) {}
    TcpHandler connection;
};

class ClientsManager
{
public:
    ClientsManager(boost::asio::io_service& io);
    ~ClientsManager();

    // Start a new thread to listen to and accept clients
    void StartListening(const tcp::endpoint &localEndpoint);
    // Start another thread to process each client
    void StartProcessing();
    // Get the connected clients
    std::vector<ClientInfo>& GetClients() { return m_clients; }
   
private:
    boost::asio::io_service &m_ioService;
    TcpListener m_listener;
    RequestHandler m_requests;
    bool m_lock;

    // Clients List
    std::vector<ClientInfo> m_clients;
    std::unordered_map<uint32_t, std::vector<unsigned int>> m_groups; // map groupId to a list of id's of clients
                                                                      //  each client id is index of m_clients vector

    // Handle a newly connected client
    void HandleClient(boost::shared_ptr<tcp::socket> &socket);
    // Process each client
    void ProcessClients();

    // Test Method to receive chat messages on GROUP_CHAT request
    void ReceiveChat(unsigned int client, unsigned int group);
};