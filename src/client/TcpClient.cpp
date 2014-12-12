#include <common/common.h>
#include <client/TcpClient.h>

TcpClient::TcpClient(boost::asio::io_service &io)
:m_io(io), m_listener(io), m_handler(io)
{}

TcpClient::~TcpClient()
{}

void TcpClient::StartListening(const tcp::endpoint& server)
{
    m_listener.Initialize(server);
    m_listener.Listen(boost::bind(&TcpClient::ListenerHandler, this, _1));
}


void TcpClient::ListenerHandler(boost::shared_ptr<tcp::socket> socket)
{
    m_handler.Initialize(socket);

    for (long long i = 0; i < 100000000; ++i);
    boost::thread t(boost::bind(&TcpClient::StartChatSession, this));
}

void TcpClient::Connect(const tcp::endpoint& server)
{
    m_handler.Initialize(server);
    boost::thread t(boost::bind(&TcpClient::StartChatSession, this));;
}

void TcpClient::StartChatSession()
{
    try
    {
        srand((unsigned int)time(NULL));

        std::stringstream ss;
        for (int i = 0; i < 10; ++i)
            ss << rand()%20 << "  ";
        std::string str = ss.str();
        std::cout << "\n\nSending Data " << str << std::endl;

        m_handler.Send(str.c_str(), str.size());

        while (true)
        {
            std::vector<char> data;
            data.resize(str.size() + 10);
            m_handler.Receive(&data[0], data.size());
            std::cout << "\n\nReceived Data ";
            for (unsigned i = 0; i < data.size(); ++i)
                std::cout << data[i];
            std::cout << std::endl;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

