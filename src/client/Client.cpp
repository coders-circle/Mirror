#include <common/common.h>
#include <client/Client.h>
#include <common/ChatMessage.h>

Client::Client()
{}

Client::~Client()
{}

// Connecting to some remote peer (server/client)
size_t Client::Connect(const tcp::endpoint& peer, bool* successful)
{
    // Lock the connections list not to process it while adding a connection
    boost::lock_guard<boost::mutex> guard(m_mutex);

    // Add new connection
    m_connections.push_back(Connection(m_io));
    TcpHandler &handler = m_connections[m_connections.size() - 1].tcpHandler;
    handler.Initialize(peer);

    std::cout << "Connected to: " << handler.GetDestinationAddress() << std::endl;
    // Set successful variable to true
    if (successful)
        *successful = true;
    return m_connections.size() - 1;
}

// Perform above function asynchronously
void Client::ConnectAsync(const tcp::endpoint& peer, bool* successful, size_t* connectionId)
{
    boost::thread t([this, peer, successful, connectionId](){
        size_t id = Connect(peer, successful);
        if (connectionId)
            *connectionId = id;
    });
}

// Use server to connect to a client (for P2P)
size_t Client::Connect(uint32_t clientId, bool* successful)
{
    // make sure no requests are being processed during this time
    boost::lock_guard<boost::mutex> guard(m_mutex);

    // First send a P2P tcp connection request to server and receive back the returning request
    TcpHandler& handler = m_connections[0].tcpHandler;
    m_request.P2PTcp(handler, clientId, handler.GetSocket()->local_endpoint().address().to_string(), handler.GetSocket()->local_endpoint().port());
    m_request.ReceiveRequest(handler);

    // If invalid request return now
    if (m_request.GetRequestType() != TcpRequest::P2P_TCP)
    {
        if (successful)
            *successful = false;
        return 0;
    }

    // Use the return request parameters to try connecting to other peer
    return HandleP2PRequest(m_request.GetClientId(),
        tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetPrivateIp()), m_request.GetPrivatePort()),
        tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetPublicIp()), m_request.GetPublicPort()), successful);
}

void Client::ConnectAsync(uint32_t clientId, bool* successful, size_t* connectionId)
{
    boost::thread t([this, clientId, successful, connectionId](){
        size_t id = Connect(clientId, successful);
        if (connectionId)
            *connectionId = id;
    });
}
size_t Client::HandleP2PRequest(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint, bool* successful)
{
    m_p2pConnecting = true;
    // The '0' connection is assumed to be a TCP connection with the server
    TcpHandler& handler = m_connections[0].tcpHandler;
    // Start listening at same local endpoint as which is connected to the server
    boost::thread t1(boost::bind(&Client::P2PListen, this, _1), handler.GetSocket()->local_endpoint());
    // Try connecting to both private and public endpoints of other peer
    if (privateEndpoint != publicEndpoint)
        boost::thread t2(boost::bind(&Client::P2PConnect, this, _1), privateEndpoint);
    boost::thread t3(boost::bind(&Client::P2PConnect, this, _1), publicEndpoint);

    /*
    TODO: Wait for connection for certain time only after which, set m_p2pConnecting to false
    */
    while (m_p2pConnecting);
    try
    {
        if (m_acceptor)
            m_acceptor->close();
    }
    catch (...) {}
    // Set successful variable to true
    if (successful)
    {
        *successful = true;
        std::cout << "Connected to client #" << clientId << " at connection: #" << m_connections.size() - 1;
    }
    return m_connections.size() - 1;
}

void Client::HandleP2PRequestAsync(uint32_t clientId, const tcp::endpoint &privateEndpoint, const tcp::endpoint &publicEndpoint, bool* successful)
{
    boost::thread t([this, clientId, privateEndpoint, publicEndpoint, successful]() /* C++11 lambda function */
    {
        try
        {
            boost::lock_guard<boost::mutex> guard(m_mutex);
            HandleP2PRequest(clientId, privateEndpoint, publicEndpoint, successful);
        }
        catch (std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    });
}

void Client::P2PListen(const tcp::endpoint &localEndpoint)
{
    try
    {
        m_acceptor.reset(new tcp::acceptor(m_io, localEndpoint, true));
        // Create a new socket to represent a new connection
        boost::shared_ptr<tcp::socket> socket(new tcp::socket(m_acceptor->get_io_service()));
        // Wait for a connection and accept at the socket
        m_acceptor->accept(*socket);
        if (!m_p2pConnecting) return;

        /*
        TODO: Need to verify if this is the required peer
        */
        Connection c(m_io);
        c.tcpHandler.Initialize(socket);
        m_connections.push_back(c);
        m_p2pConnecting = false;
    }
    catch (/*std::exception &ex*/...)
    {
        //std::cout << "Listener: " << ex.what() << std::endl;
    }
}
;
void Client::P2PConnect(tcp::endpoint &remoteEndpoint)
{
    while (m_p2pConnecting)
    {
        try
        {
            // Try connecting at the given remote endpoint
            Connection c(m_io);
            c.tcpHandler.Initialize(remoteEndpoint);
            if (!m_p2pConnecting) return;

            m_connections.push_back(c);
            m_p2pConnecting = false;
        }
        catch (/*std::exception &ex*/...)
        {
            //std::cout << "Connector: " << ex.what() << std::endl;
        }
    }
}

void Client::HandleRequestsAsync() 
{ 
    boost::thread t(boost::bind(&Client::HandleRequests, this));
}

// An infinite loop to handle incoming requests
void Client::HandleRequests()
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
                bool joinChat;
                // if so, process accordingly
                if (bytes > 0)
                {
                    m_request.ReceiveRequest(m_connections[i].tcpHandler);
                    switch (m_request.GetRequestType())
                    {
                    // Request to join chat conversation
                    case TcpRequest::JOIN_CHAT:
                        if (m_joinChatHandler)
                            joinChat = m_joinChatHandler(i);
                        else
                            joinChat = true;
                        if (joinChat)
                            m_request.JoinChat(m_connections[i].tcpHandler, m_request.GetGroupId());
                        else
                            m_request.Invalid(m_connections[i].tcpHandler);
                        break;

                    // Request to receive an incoming chat message
                    case TcpRequest::CHAT_MESSAGE:
                        chat.Receive(m_connections[i].tcpHandler, m_request.GetMessageSize());
                        if (m_messageHandler)
                        {
                            boost::shared_ptr<MessageEventData> msgData(new MessageEventData);
                            msgData->senderId = m_request.GetUserId();
                            msgData->connectionId = i;
                            msgData->message = chat.GetMessage();
                            m_messageHandler(msgData);
                        }
                        break;

                    // Request for a p2p tcp connection
                    case TcpRequest::P2P_TCP:
                        id = m_request.GetClientId();
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

bool Client::JoinChat(uint32_t connectionId, uint32_t groupId)
{
    m_request.JoinChat(m_connections[connectionId].tcpHandler, groupId);
    // We need a JOIN_CHAT request back from other peer/server to ensure chat is joined
    m_request.ReceiveRequest(m_connections[connectionId].tcpHandler);
    if (m_request.GetRequestType() != TcpRequest::JOIN_CHAT)
        return false;
    return true;
}

void Client::SendMessage(size_t receiverId, const std::string& message, uint32_t groupId)
{
    if (receiverId >= m_connections.size())
        throw Exception("Invalid Connection-Id to send message to");
    // Send the chat message after a CHAT_MESSAGE request
    m_request.ChatMessage(m_connections[receiverId].tcpHandler, message.size() + 1, m_name, groupId);
    ChatMessage chat;
    chat.SetMessage(message);
    chat.Send(m_connections[receiverId].tcpHandler);
}
