#pragma once
#include <common/TcpAcceptor.h>
#include <common/TcpHandler.h>
#include <common/UdpHandler.h>
#include <common/TcpRequest.h>

struct ClientInfo
{
    ClientInfo(boost::asio::io_service& io) 
    : tcpHandler(io) {}
    TcpHandler tcpHandler;
    udp::endpoint udpEndpoint1, udpEndpoint2;
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

    // Udp Connections
    UdpHandler m_udpHandler1;
    UdpHandler m_udpHandler2;

    // Clients List
    std::vector<ClientInfo> m_clients;
    std::map<std::pair<std::string, uint16_t>, size_t> m_udpEndpointsMap;
    std::map<size_t, size_t> m_videoGroups; // map client to group that it is video-chatting with

    std::unordered_map<uint32_t, std::vector<unsigned int>> m_groups; // map groupId to a list of id's of clients
                                                                      //  each client id is index of m_clients vector

    // Handle a newly connected client
    void HandleClient(boost::shared_ptr<tcp::socket> &socket);
    // Process each client
    void ProcessClients();

    // Test Method to receive chat messages on GROUP_CHAT request
    void ReceiveChat(unsigned int client, unsigned int group);

    static std::pair<std::string, uint16_t> GetEndpointKey(const udp::endpoint& ep)
    {
        return std::make_pair(ep.address().to_string(), ep.port());
    }
    // Disconnect a client
    void Disconnect(size_t client);
};
