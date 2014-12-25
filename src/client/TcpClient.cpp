#include <common/common.h>
#include <client/TcpClient.h>
#include <common/ChatMessage.h>

TcpClient::TcpClient(boost::asio::io_service &io)
:m_io(io), /*m_acceptor(io),*/ m_currentConnection(-1)
{}

TcpClient::~TcpClient()
{}

// Connecting to some remote peer (server/client)
void TcpClient::Connect(const tcp::endpoint& peer, bool joinChat)
{
    // Lock the connections list not to process it while adding a connection
    boost::lock_guard<boost::mutex> guard(m_mutex); 

    // Add new connection
    m_connections.push_back(Connection(m_io));
    TcpHandler &handler = m_connections[m_connections.size() - 1].tcpHandler;
    handler.Initialize(peer);

    std::cout << "Connected to: " << handler.GetDestinationAddress() << std::endl;
    // Join chat in the new connection
    if (joinChat)
        JoinChat(m_connections.size() - 1, 0);
}

// Perform above function asynchronously
void TcpClient::ConnectAsync(const tcp::endpoint& peer, bool joinChat)
{
    boost::thread t(boost::bind((void(TcpClient::*)(const tcp::endpoint&, bool))&TcpClient::Connect, this, _1, _2), peer, joinChat);
}

// Use server to connect to a client (for P2P)
void TcpClient::Connect(uint32_t clientId)
{
    m_p2pConnected = false;
    // make sure no requests are being processed during this time
    boost::lock_guard<boost::mutex> guard(m_mutex);

    // First send a P2P tcp connection request to server and receive back the returning request
    TcpHandler& handler = m_connections[0].tcpHandler;
    m_request.P2PTcp(handler, clientId, handler.GetSocket()->local_endpoint().address().to_string(), handler.GetSocket()->local_endpoint().port());
    m_request.ReceiveRequest(handler);

    HandleP2PRequest(m_request.GetClientId(), 
        tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetPrivateIp()), m_request.GetPrivatePort()),
        tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetPublicIp()), m_request.GetPublicPort()));
}

void TcpClient::HandleP2PRequest(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint)
{
    TcpHandler& handler = m_connections[0].tcpHandler;
    // Start listening at same local endpoint as which is connected to the server
    boost::thread t1(boost::bind(&TcpClient::P2PListen, this, _1), handler.GetSocket()->local_endpoint());
    // Try connecting to both private and public endpoints of other peer
    if (privateEndpoint!=publicEndpoint)
        boost::thread t2(boost::bind(&TcpClient::P2PConnect, this, _1), privateEndpoint);
    boost::thread t3(boost::bind(&TcpClient::P2PConnect, this, _1), publicEndpoint);

    while (!m_p2pConnected);
    if (m_acceptor)
        m_acceptor->cancel();
}

void TcpClient::HandleP2PRequestAsync(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint)
{
    boost::thread t([this, clientId, privateEndpoint, publicEndpoint]()
    {
        try
        {
            boost::lock_guard<boost::mutex> guard(m_mutex);
            HandleP2PRequest(clientId, privateEndpoint, publicEndpoint);
        }
        catch (std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    });
}

void TcpClient::P2PListen(const tcp::endpoint &localEndpoint)
{
    try
    {
        m_acceptor.reset(new tcp::acceptor(m_io, localEndpoint, true));
        // Create a new socket to represent a new connection
        boost::shared_ptr<tcp::socket> socket(new tcp::socket(m_acceptor->get_io_service()));
        // Wait for a connection and accept at the socket
        m_acceptor->accept(*socket);
        if (m_p2pConnected) return;   

        /*
        TODO: Need to verify if this is the required peer
        */
        Connection c(m_io);
        c.tcpHandler.Initialize(socket);
        m_connections.push_back(c);
        m_p2pConnected = true;
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
;
void TcpClient::P2PConnect(tcp::endpoint &remoteEndpoint)
{
    while (!m_p2pConnected)
    {
        try
        {
            // Try connecting at the given remote endpoint
            Connection c(m_io);
            c.tcpHandler.Initialize(remoteEndpoint);
            if (m_p2pConnected) return;

            m_connections.push_back(c);
            m_p2pConnected = true;
            JoinChat(m_connections.size() - 1, 0);
        }
        catch (std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }
}

// An infinite loop to handle incoming requests
void TcpClient::HandleRequests()
{
    while (true)
    {
        // Sleep for some time if there is not connection
        while (m_connections.size() == 0)
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

        // While processing connections, lock the list so no new connection is added at the time
        m_mutex.lock();
        for (unsigned int i = 0; i < m_connections.size(); ++i)
        {
            try
            {
                // See if any request is incomming for this connection
                size_t bytes = m_connections[i].tcpHandler.Available();
                ChatMessage chat;
                uint32_t id;
                // if so, process accordingly
                if (bytes > 0)
                {
                    m_request.ReceiveRequest(m_connections[i].tcpHandler);
                    switch (m_request.GetRequestType())
                    {
                    // Request to join chat conversation
                    case TcpRequest::JOIN_CHAT:
                        m_currentConnection = i;
                        break;
                    // Request to wait for incoming chat message
                    case TcpRequest::CHAT_MESSAGE:
                        chat.Receive(m_connections[i].tcpHandler, m_request.GetMessageSize());
                        m_currentConnection = i;
                        std::cout << "\n\n" << chat.GetMessage() << "\n\nYou: ";
                        break;
                    // Request for a p2p tcp connection
                    case TcpRequest::P2P_TCP:
                        id = m_request.GetClientId();
                        m_p2pConnected = false;
                        HandleP2PRequestAsync(id,
                            tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetPrivateIp()), m_request.GetPrivatePort()),
                            tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetPublicIp()), m_request.GetPublicPort()));
                        m_request.P2PTcp(m_connections[0].tcpHandler, id,
                            m_connections[0].tcpHandler.GetSocket()->local_endpoint().address().to_string(), m_connections[0].tcpHandler.GetSocket()->local_endpoint().port()
                        );
                        break;
                    default:
                        std::cout << "Invalid Request " << m_request.GetRequestType() << std::endl;
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

void TcpClient::StartChatInput(uint32_t groupId)
{
    boost::thread t(boost::bind(&TcpClient::ChatInput, this, _1), groupId);
}

void TcpClient::JoinChat(uint32_t connectionId, uint32_t groupId)
{
    m_request.JoinChat(m_connections[connectionId].tcpHandler, groupId);
    m_currentConnection = connectionId;
}

void TcpClient::ChatInput(uint32_t groupId)
{
    while (true)
    {
        while (m_currentConnection == -1)
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        
        // Take input
        std::cout << "\nYou: ";
        fflush(stdin);
        char input[1024];

        std::cin.getline(input, 1024);
        std::string message = m_name + ": " + input;

        // Send the chat message after a GROUP_CHAT request
        m_request.ChatMessage(m_connections[m_currentConnection].tcpHandler, message.size() + 1, groupId);
        ChatMessage chat;
        chat.SetMessage(message);
        chat.Send(m_connections[m_currentConnection].tcpHandler);
    }
}
