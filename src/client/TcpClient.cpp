#include <common/common.h>
#include <client/TcpClient.h>

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

    // For test purpose, we start a new session to send/receive random data
    // The random data is seeded with current time
    // Since two client applications are opened and run simultaneously
    //  both generate same random data as they are connected at same time
    // To solve this issue, in the listening client, we make some delay
    // In the client that sends the request for connection,
    //  we make no such delay (see below)
    //for (long long i = 0; i < 100000000; ++i)
    //    ;
    // Now start the new thread to send/receive data
    //boost::thread t(boost::bind(&TcpClient::StartChatSession, this));
}

void TcpClient::Connect(const tcp::endpoint& peer)
{
    m_tcpHandler.Initialize(peer);
    std::cout << "Connected: " << m_tcpHandler.GetDestinationAddress() << std::endl;

    // For test purpose, start a new thread to send/receive data
    //boost::thread t(boost::bind(&TcpClient::StartChatSession, this));;
}

void TcpClient::JoinGroup(uint32_t groupId)
{
    m_requests.JoinGroup(m_tcpHandler, groupId);
}

#include <common/ChatMessage.h>
// Send/Receive randome data
void TcpClient::StartChatSession()
{
    try
    {
        // Generate a string with random numbers
        srand((unsigned int)time(NULL));
        std::stringstream ss;
        for (int i = 0; i < 10; ++i)
            ss << rand()%20 << "  ";
        std::string str = ss.str();
        std::cout << "\n\nSending Data " << str << std::endl;


        // Send the string as a chat message
        ChatMessage message;
        message.SetMessage(str);
        message.Send(m_tcpHandler);


        // keep receiving messages and print them
        while (true)
        {
            message.Receive(m_tcpHandler);
            std::cout << "\n\nReceived Data ";
            std::cout << message.GetMessage();
            std::cout << std::endl;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

