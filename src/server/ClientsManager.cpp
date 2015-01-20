#include <common/common.h>
#include <server/ClientsManager.h>
#include <common/ChatMessage.h>

ClientsManager::ClientsManager(boost::asio::io_service& io)
: m_acceptor(io), m_ioService(io), m_udpHandler1(io), m_udpHandler2(io)
{
    m_udpHandler1.Initialize(udp::endpoint(udp::v4(), 1106));
    m_udpHandler2.Initialize(udp::endpoint(udp::v4(), 1107));
    std::cout << "Udp Ports Used: \n" << m_udpHandler1.GetSocket()->local_endpoint().port() 
                << ", " << m_udpHandler2.GetSocket()->local_endpoint().port() << std::endl;
}

ClientsManager::~ClientsManager()
{}

// Use listener to listen to incoming clients
void ClientsManager::StartListening(const tcp::endpoint &localEndpoint)
{
    m_acceptor.Initialize(localEndpoint);
    m_acceptor.Listen(boost::bind(&ClientsManager::HandleClient, this, _1));
}

// Add any client that is connected to the clients list
void ClientsManager::HandleClient(boost::shared_ptr<tcp::socket> &socket)
{
    boost::lock_guard<boost::mutex> guard(m_mutex);
    size_t i = 0;
    // if some client is disconnected, we replace it with new client
    for (; i < m_clients.size(); ++i)
        if (!m_clients[i].connected)
            break;
    
    if (i < m_clients.size())
        m_clients[i].tcpHandler.ResetSocket();
    else
    {
        // If no disconnected client is found, we add a new client to the list
        ClientInfo client(m_ioService);
        m_clients.push_back(client);
        i = m_clients.size() - 1;
    }
    // Initialize the tcp handler for the client, and set connected status to true
    m_clients[i].tcpHandler.Initialize(socket);
    m_clients[i].connected = true;
    m_clients[i].udpEndpoint1 = m_clients[i].udpEndpoint2 = udp::endpoint();

    std::cout << "Client #" << i << " Connected: " << m_clients[i].tcpHandler.GetDestinationAddress() << std::endl;
}

// Start processing each client
void ClientsManager::StartProcessing()
{
    //boost::thread t(boost::bind(&ClientsManager::ProcessClients, this));
    ProcessClients();
}


void ClientsManager::Disconnect(size_t client)
{
    m_clients[client].tcpHandler.Close();
    m_clients[client].connected = false;
    auto it = m_videoGroups.find(client);
    if (it != m_videoGroups.end())
        m_videoGroups.erase(it);
    auto it1 = m_udpEndpointsMap.find(GetEndpointKey(m_clients[client].udpEndpoint1));
    if (it1 != m_udpEndpointsMap.end())
        m_udpEndpointsMap.erase(it1);
}

// Process each client
void ClientsManager::ProcessClients()
{
    // TODO: Disconnect client when long in idle state (i.e. when no data available for long time)

    while (true)
    {
        // Sleep for some time if no client is available
        while (m_clients.size() == 0)
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

        // Lock the clients-list while processing the clients
        m_mutex.lock();
        // Process each client in turn
        for (unsigned int i = 0; i < m_clients.size(); ++i)
        {
            if (!m_clients[i].connected) continue;
            try
            {
                uint32_t id;
                // See if any request is incomming for this client
                size_t bytes = m_clients[i].tcpHandler.Available();
                if (bytes > 0)
                {
                    // if so receive the request and process accordingly
                    m_requests.ReceiveRequest(m_clients[i].tcpHandler);

                    switch (m_requests.GetRequestType())
                    {
                    // Request to join chat, since this is the server, this is a request to join a group chat
                    case TcpRequest::JOIN_CHAT:
                        id = m_requests.GetGroupId();
                        m_requests.JoinChat(m_clients[i].tcpHandler, id);
                        // push the client id to the group
                        m_groups[id].push_back(i);
                        std::cout << "Connected client #" << i << " to group #" << id << std::endl;
                        break;

                    // Request to receive an incoming chat message
                    case TcpRequest::CHAT_MESSAGE:
                        id = m_requests.GetGroupId();
                        // receive the chat message
                        ReceiveChat(i, id);
                        break;
                    
                    // Request for a p2p tcp connection to another client
                    case TcpRequest::P2P_TCP:
                        id = m_requests.GetClientId();
                        if (id < m_clients.size() && id != i)
                        {
                            // Send request to second client
                            m_requests.P2PTcp(m_clients[id].tcpHandler, i, m_requests.GetPrivateIp(), m_requests.GetPrivatePort(),
                                m_clients[i].tcpHandler.GetRemoteIp(), m_clients[i].tcpHandler.GetRemotePort());
                            // Receive the return request
                            m_requests.ReceiveRequest(m_clients[id].tcpHandler);
                            // Send back the return request to first client
                            m_requests.P2PTcp(m_clients[i].tcpHandler, id, m_requests.GetPrivateIp(), m_requests.GetPrivatePort(),
                                m_clients[id].tcpHandler.GetRemoteIp(), m_clients[id].tcpHandler.GetRemotePort());
                        }
                        else
                        {
                            // Send an invalid request
                            m_requests.Invalid(m_clients[i].tcpHandler);
                        }
                        break;
                    
                    // Request for disconnection
                    case TcpRequest::DISCONNECT:
                        m_clients[i].tcpHandler.Close();
                        m_clients[i].connected = false;
                        break;
                    
                    // Request to connect to send udp port of this server and for an udp connection
                    case TcpRequest::UDP_PORT:
                    {   
                        // Send the even udp port
                        m_requests.UdpPort(m_clients[i].tcpHandler, m_udpHandler1.GetSocket()->local_endpoint().port());
                        // The client now tries to connect to the port by sending a byte from the client's even udp port
                        // Receive the one byte sent by client to deduce its address-port pair
                        char c;
                        m_udpHandler1.Receive(m_clients[i].udpEndpoint1, &c, 1);
                        // The odd udp port of client is its event port + 1
                        m_clients[i].udpEndpoint2 = udp::endpoint(m_clients[i].udpEndpoint1.address(), m_clients[i].udpEndpoint1.port()+1);
                        // To later recognize who send the udp data, we keep mapping of udp-endpoints to client-ids
                        m_udpEndpointsMap[GetEndpointKey(m_clients[i].udpEndpoint1)] = i;
                        std::cout << "UDP Connection with client #" << i << " established at client's port: " << m_clients[i].udpEndpoint1.port() << std::endl;
                    }
                        break;
                    
                    // Request for a video chat
                    case TcpRequest::JOIN_VIDEO_CHAT:
                        id = m_requests.GetGroupId();
                        // Make sure client has joined the group already (through JOIN_CHAT request)
                        if (std::find(m_groups[id].begin(), m_groups[id].end(), i) == m_groups[id].end())
                            m_requests.Invalid(m_clients[i].tcpHandler);
                        else
                        {
                            // Send back the client a video chat request, which the client is waiting for
                            m_requests.JoinVideoChat(m_clients[i].tcpHandler, id);
                            m_videoGroups[i] = id;
                            std::cout << "Client #" << i << " is video chatting in group #" << id << std::endl;
                        }
                        break;

                    default:
                        std::cout << "Invalid Request " << m_requests.GetRequestType() << " from client #" << i << std::endl;
                    }
                }
            }
            catch (std::exception &ex)
            {
               Disconnect(i);
               std::cout << ex.what() << std::endl;
            }
        }
        
        // Also check for udp data
        if (m_udpHandler1.GetSocket()->available() > 0)
        {
            // Let max udp-packet size be 1500; receive the data through the udpHandler
            udp::endpoint ep;
            char data[1500];
            size_t len = m_udpHandler1.Receive(ep, data, 1500);
            if (len != 0)
            {
                // To find the client who send the data, we use the endpoint-clientId mapping
                size_t cid = m_udpEndpointsMap[GetEndpointKey(ep)];
                // Find which video group the client is connected at
                auto it = m_videoGroups.find(cid);
                if (it != m_videoGroups.end())
                for (unsigned int j = 0; j < m_groups[it->second].size(); ++j)
                    //if (m_groups[it->second][j] != cid)
                    // Send to each peer in the video-chat group the data received
                    m_udpHandler1.Send(m_clients[m_groups[it->second][j]].udpEndpoint1, data, len);
            }
        }

        m_mutex.unlock();
    }
}

void ClientsManager::ReceiveChat(unsigned int client, unsigned int group)
{
    ChatMessage chat;
    chat.Receive(m_clients[client].tcpHandler, m_requests.GetMessageSize());

    // Send the messsage to each client in the group
    for (unsigned int i = 0; i < m_groups[group].size(); ++i)
    {
        try
        {
            if (m_groups[group][i] != client)
            {
                m_requests.ChatMessage(m_clients[i].tcpHandler, chat.GetMessage().size() + 1, m_requests.GetUserId());
                chat.Send(m_clients[m_groups[group][i]].tcpHandler);
            }
        }
        // client may be disconnected and exception may be thrown
        //  we catch the exception inside the loop so that
        //  we can continue sending messages to other clients
        catch (std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }
}

