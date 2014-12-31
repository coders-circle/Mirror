#pragma once
#include <common/TcpAcceptor.h>
#include <common/TcpHandler.h>
#include <common/TcpRequest.h>

struct ClientInfo
{
    ClientInfo(boost::asio::io_service& io) : connection(io) {}
    TcpHandler connection;
    bool connected;
};

class ClientsManager
{
public:
    ClientsManager(boost::asio::io_service& io);
    ~ClientsManager();

    // Start a new thread to listen to and accept clients
    void StartListening(const tcp::endpoint &localEndpoint);
    // Start an infinite loop to process each client
    void StartProcessing();
    // Get the connected clients
    std::vector<ClientInfo>& GetClients() { return m_clients; }
   
private:
    boost::asio::io_service &m_ioService;
    TcpAcceptor m_acceptor;
    TcpRequest m_requests;
    boost::mutex m_mutex;

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
