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
    std::vector<ClientInfo>& GetClients() { return m_clients; }
   
private:
    TcpListener m_listener;
    boost::asio::io_service &m_ioService;
    RequestHandler m_requests;
    bool m_lock;

    // Clients List
    std::vector<ClientInfo> m_clients;
    std::unordered_map<uint32_t, std::vector<unsigned int>> m_groups; // map groupId to a list of id's of clients
                                                                      //  each client id is index of m_clients vector

    void HandleClient(boost::shared_ptr<tcp::socket> &socket);
    void ProcessClients();
};