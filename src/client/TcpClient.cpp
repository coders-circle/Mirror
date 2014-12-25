#include <common/common.h>
#include <client/TcpClient.h>
#include <common/ChatMessage.h>

TcpClient::TcpClient(boost::asio::io_service &io)
:m_io(io), m_acceptor(io), m_currentConnection(-1)
{}

TcpClient::~TcpClient()
{}

void TcpClient::StartAccepting()
{
    uint16_t port = m_acceptor.Initialize(tcp::endpoint(tcp::v4(), 0));
    std::cout << "Listening at port: " << port << std::endl;
   
    // Listen method will start listening in new thread
     m_acceptor.Listen(boost::bind(&TcpClient::AcceptorHandler, this, _1));
}

// This is the handler called when the acceptor accepts a new connection
void TcpClient::AcceptorHandler(boost::shared_ptr<tcp::socket> socket)
{
    // Lock the connections list not to process it while adding a connection
    boost::lock_guard<boost::mutex> guard(m_connectionsMutex);

    // Add new connection
    m_connections.push_back(Connection(m_io));
    TcpHandler &handler = m_connections[m_connections.size() - 1].tcpHandler;
    handler.Initialize(socket);
    
    std::cout << "Connected to: " << handler.GetDestinationAddress() << std::endl;
}

// Connecting to some remote peer (server/client)
void TcpClient::Connect(const tcp::endpoint& peer, bool joinChat)
{
    // Lock the connections list not to process it while adding a connection
    boost::lock_guard<boost::mutex> guard(m_connectionsMutex);

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
    // Send a request to get a client-address; assuming server id first element in the connections-list
    m_request.SendClientAddr(m_connections[0].tcpHandler, clientId);

    // Receive for the response and use it to connect to the client
    m_request.ReceiveRequest(m_connections[0].tcpHandler);
    if (m_request.GetRequestType() != TcpRequest::RECEIVE_CLIENT_ADDR)
        throw Exception("Expecting a client address but getting some other request");
    Connect(tcp::endpoint(boost::asio::ip::address::from_string(m_request.GetIp()), m_request.GetPort()), true);
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
        m_connectionsMutex.lock();
        for (unsigned int i = 0; i < m_connections.size(); ++i)
        {
            try
            {
                // See if any request is incomming for this connection
                size_t bytes = m_connections[i].tcpHandler.Available();
                ChatMessage chat;
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
                    // Request to send the client address
                    case TcpRequest::SEND_CLIENT_ADDR:
                        // Since this IS the client, the other connection already has an IP
                        // There is no need to send ip, just the port
                        m_request.ReceiveClientAddr(m_connections[i].tcpHandler, m_request.GetClientId(), "", m_acceptor.GetSocket().local_endpoint().port());
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
        m_connectionsMutex.unlock();
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
