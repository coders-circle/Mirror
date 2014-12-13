#include <common/common.h>
#include <client/TcpClient.h>
#include <common/ChatMessage.h>

TcpClient::TcpClient(boost::asio::io_service &io)
:m_io(io), m_listener(io), m_tcpHandler(io)
{}

TcpClient::~TcpClient()
{}

void TcpClient::StartListening(const tcp::endpoint& localEndPoint)
{
    m_listener.Initialize(localEndPoint);
    // Listen method will start listening in new thread
    m_listener.Listen(boost::bind(&TcpClient::ListenerHandler, this, _1));
}

// This is the handler called when the listener accepts a new connection
void TcpClient::ListenerHandler(boost::shared_ptr<tcp::socket> socket)
{
    m_tcpHandler.Initialize(socket);
    std::cout << "Connected: " << m_tcpHandler.GetDestinationAddress() << std::endl;
}

// Connect to some remote peer (server/client)
void TcpClient::Connect(const tcp::endpoint& peer)
{
    m_tcpHandler.Initialize(peer);
    std::cout << "Connected: " << m_tcpHandler.GetDestinationAddress() << std::endl;
}

// Join group by sending request to server
void TcpClient::JoinGroup(uint32_t groupId)
{
    m_requests.JoinGroup(m_tcpHandler, groupId);
}

// Start group chat in a new thread
void TcpClient::StartChatSession(uint32_t groupId)
{
    boost::thread t(boost::bind(&TcpClient::ChatSession, this, _1), groupId);
}

void TcpClient::ChatSession(uint32_t groupId)
{
    try
    {
        std::cout << "\n\n";
        // A new thread to input chat messages
        boost::thread t(boost::bind(&TcpClient::ChatInput, this, _1), groupId);

        // In this thread, keep receiving messages and print them
        ChatMessage chat;
        while (true)
        {
            chat.Receive(m_tcpHandler);
            std::cout << "\n\n" << chat.GetMessage() << "\n\nYou: ";
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

void TcpClient::ChatInput(uint32_t groupId)
{
    while (true)
    {
        // Take input
        std::cout << "\nYou: ";
        fflush(stdin);
        char input[1024];

        std::cin.getline(input, 1024);
        std::string message = std::to_string(m_tcpHandler.GetSocket()->local_endpoint().port()) + ": " + input;

        // Send the chat message after a GROUP_CHAT request
        m_requests.GroupChat(m_tcpHandler, groupId);
        ChatMessage chat;
        chat.SetMessage(message);
        chat.Send(m_tcpHandler);
    }
}
