#include <common/common.h>
#include <server/ClientsManager.h>
#include <common/ChatMessage.h>

ClientsManager::ClientsManager(boost::asio::io_service& io)
: m_acceptor(io), m_ioService(io)
{}

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
    client.connection.Initialize(socket);
    m_clients.push_back(client);
    std::cout << "Client #" << m_clients.size() - 1 << " Connected: " << m_clients[m_clients.size() - 1].connection.GetDestinationAddress() << std::endl;
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
            try
            {
                uint32_t id;
                // See if any request is incomming for this client
                size_t bytes = m_clients[i].connection.Available();
                if (bytes > 0)
                {
                    // if so receive the request and process accordingly
                    m_requests.ReceiveRequest(m_clients[i].connection);

                    switch (m_requests.GetRequestType())
                    {
                    // Request to join chat, since this is the server, this is a request to join a group chat
                    case TcpRequest::JOIN_CHAT:
                        id = m_requests.GetGroupId();
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
                            m_requests.P2PTcp(m_clients[id].connection, i, m_requests.GetPrivateIp(), m_requests.GetPrivatePort(),
                                m_clients[i].connection.GetIp(), m_clients[i].connection.GetPort());
                            // Receive the return request
                            m_requests.ReceiveRequest(m_clients[id].connection);
                            // Send back the return request to first client
                            m_requests.P2PTcp(m_clients[i].connection, id, m_requests.GetPrivateIp(), m_requests.GetPrivatePort(),
                                m_clients[id].connection.GetIp(), m_clients[id].connection.GetPort());
                        }
                        else
                        {
                            // Send an invalid request
                            m_requests.Invalid(m_clients[i].connection);
                        }
                        break;

                    default:
                        std::cout << "Invalid Request " << m_requests.GetRequestType() << " from client #" << i << std::endl;
                    }
                }
            }
            catch (std::exception &ex)
            {
                std::cout << ex.what() << std::endl;
            }
        }
        m_mutex.unlock();
    }
}

void ClientsManager::ReceiveChat(unsigned int client, unsigned int group)
{
    ChatMessage chat;
    chat.Receive(m_clients[client].connection, m_requests.GetMessageSize());
    //m_mutex.lock();

    // Send the messsage to each client in the group
    for (unsigned int i = 0; i < m_groups[group].size(); ++i)
    {
        try
        {
            if (m_groups[group][i] != client)
            {
                m_requests.ChatMessage(m_clients[i].connection, chat.GetMessage().size() + 1);
                chat.Send(m_clients[m_groups[group][i]].connection);
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