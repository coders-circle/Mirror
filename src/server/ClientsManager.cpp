#include <common/common.h>
#include <server/ClientsManager.h>

ClientsManager::ClientsManager(boost::asio::io_service& io)
: m_listener(io), m_ioService(io)
{}

ClientsManager::~ClientsManager()
{}

void ClientsManager::StartListening(const tcp::endpoint &localEndpoint)
{
    m_listener.Initialize(localEndpoint);
    m_listener.Listen(boost::bind(&ClientsManager::HandleClient, this, _1));
}

// Add any client that is connected to the clients list
void ClientsManager::HandleClient(boost::shared_ptr<tcp::socket> &socket)
{
    m_lock = true;
    ClientInfo client(m_ioService);
    client.connection.Initialize(socket);
    m_clients.push_back(client);
    m_lock = false;
}

void ClientsManager::StartProcessing()
{
    m_lock = false;
    boost::thread t(boost::bind(&ClientsManager::ProcessClients, this));
}

void ClientsManager::ProcessClients()
{
    while (true)
    {
        while (m_lock);
        for (unsigned int i = 0; i < m_clients.size(); ++i)
        {
            while (m_lock);
            try
            {
                size_t bytes = m_clients[i].connection.Available();
                if (bytes > 0)
                {
                    m_requests.ReceiveRequest(m_clients[i].connection);
                    if (m_requests.GetLastRequest().type == RequestHandler::JOIN_GROUP)
                    {
                        uint32_t gid = m_requests.GetLastRequest().info.join.groupId;
                        m_groups[gid].push_back(i);   // push the client id to the group
                        std::cout << "Connected client #" << i << " to group #" << gid << std::endl;
                    }
                }
            }
            catch (std::exception &ex)
            {}
        }
    }
}