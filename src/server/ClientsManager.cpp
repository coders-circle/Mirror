#include <common/common.h>
#include <server/ClientsManager.h>
#include <common/ChatMessage.h>

ClientsManager::ClientsManager(boost::asio::io_service& io)
: m_listener(io), m_ioService(io)
{}

ClientsManager::~ClientsManager()
{}

// Use listener to listen to incoming clients
void ClientsManager::StartListening(const tcp::endpoint &localEndpoint)
{
    m_listener.Initialize(localEndpoint);
    m_listener.Listen(boost::bind(&ClientsManager::HandleClient, this, _1));
}

// Add any client that is connected to the clients list
void ClientsManager::HandleClient(boost::shared_ptr<tcp::socket> &socket)
{
    m_lock = true;      // Lock to ensure "m_clients" vector isn't used while adding client to it

    ClientInfo client(m_ioService);
    client.connection.Initialize(socket);
    m_clients.push_back(client);
    std::cout << "Client Connected: " << m_clients[m_clients.size() - 1].connection.GetDestinationAddress() << std::endl;

    m_lock = false;     // Un-lock
}

// Start processing each client
void ClientsManager::StartProcessing()
{
    m_lock = false; // Initially, the "m_clients" vector is un-locked
    boost::thread t(boost::bind(&ClientsManager::ProcessClients, this));
}

// Process each client
void ClientsManager::ProcessClients()
{
    while (true)
    {
        while (m_lock)
            ;             // Don't continue while locked

        // Process each client in turn
        for (unsigned int i = 0; i < m_clients.size(); ++i)
        {
            while (m_lock)
                ;         // Pause processing while locked
            try
            {
                // See if any request is incomming for this client
                size_t bytes = m_clients[i].connection.Available();
                uint32_t gid;
                // if so, process accordingly
                if (bytes > 0)
                {
                    m_requests.ReceiveRequest(m_clients[i].connection);
                    switch (m_requests.GetLastRequest().type)
                    {
                    case RequestHandler::JOIN_GROUP:
                        gid = m_requests.GetLastRequest().info.join.groupId;
                        // push the client id to the group
                        m_groups[gid].push_back(i);   
                        std::cout << "Connected client #" << i << " to group #" << gid << std::endl;
                        break;
                    
                    case RequestHandler::GROUP_CHAT:
                        gid = m_requests.GetLastRequest().info.groupChat.groupId;
                        // receive the chat message
                        ReceiveChat(i, gid);
                        break;

                    default:
                        std::cout << "Invalid Request Got from client #" << i << std::endl;
                    }
                }
            }
            catch (std::exception &ex)
            {
                std::cout << ex.what() << std::endl;
            }
        }
    }
}

void ClientsManager::ReceiveChat(unsigned int client, unsigned int group)
{
    ChatMessage chat;
    chat.Receive(m_clients[client].connection);
    while (m_lock)
        ;  // while locked, don't continue

    // Send the messsage to each client in the group
    for (unsigned int i = 0; i < m_groups[group].size(); ++i)
    {
        try
        {
            if (m_groups[group][i] != client)
                chat.Send(m_clients[m_groups[group][i]].connection);
            while (m_lock)
                ; // while locked, don't continue
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