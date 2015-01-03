#include <common/common.h>
#include <server/ClientsManager.h>
#include <common/ChatMessage.h>

ClientsManager::ClientsManager(boost::asio::io_service& io)
: m_acceptor(io), m_ioService(io), m_udpHandler1(io), m_udpHandler2(io)
{
    UdpHandler::GetUdpPairs(m_udpHandler1, m_udpHandler2);
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
    ClientInfo client(m_ioService);
    client.tcpHandler.Initialize(socket);
    client.connected = true;
    m_clients.push_back(client);
    std::cout << "Client #" << m_clients.size() - 1 << " Connected: " << m_clients[m_clients.size() - 1].tcpHandler.GetDestinationAddress() << std::endl;
}

// Start processing each client
void ClientsManager::StartProcessing()
{
    //boost::thread t(boost::bind(&ClientsManager::ProcessClients, this));
    ProcessClients();
}

// Process each client
void ClientsManager::ProcessClients()
{
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
                    
                    case TcpRequest::DISCONNECT:
                        m_clients[i].tcpHandler.Close();
                        m_clients[i].connected = false;
                        //m_clients.erase(m_clients.begin()+i);
                        i--;
                        break;
                    default:
                        std::cout << "Invalid Request " << m_requests.GetRequestType() << " from client #" << i << std::endl;
                    }
                }
            }
            catch (std::exception &ex)
            {
                m_clients[i].tcpHandler.Close();
                m_clients[i].connected = false;
                std::cout << ex.what() << std::endl;
            }
        }
        m_mutex.unlock();
    }
}

void ClientsManager::ReceiveChat(unsigned int client, unsigned int group)
{
    ChatMessage chat;
    chat.Receive(m_clients[client].tcpHandler, m_requests.GetMessageSize());
    //m_mutex.lock();

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
    //m_mutex.unlock();
}
