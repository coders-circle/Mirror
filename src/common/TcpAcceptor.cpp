#include <common/common.h>
#include <common/TcpAcceptor.h>

TcpAcceptor::TcpAcceptor(boost::asio::io_service &io) : m_acceptor(io)
{}

TcpAcceptor::~TcpAcceptor()
{}

uint16_t TcpAcceptor::Initialize(const tcp::endpoint &localEndpoint)
{
    // Open and bind the tcp acceptor to the local endpoint
    m_acceptor.open(localEndpoint.protocol());
    //m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(localEndpoint);
    // Prepare to listen
    m_acceptor.listen();
    return m_acceptor.local_endpoint().port();
}

void TcpAcceptor::Listen(boost::function<void(boost::shared_ptr<tcp::socket>)> callback)
{
    m_callback = callback;
    StartListening();
}

// Create new thread to listen for incomming connections
void TcpAcceptor::StartListening()
{
    boost::thread t(boost::bind(&TcpAcceptor::ListeningThread, this));
}

void TcpAcceptor::ListeningThread()
{
    try
    {
        while (true)
        {
            // Create a new socket to represent a new connection
            boost::shared_ptr<tcp::socket> socket(new tcp::socket(m_acceptor.get_io_service()));
            // Wait for a connection and accept at the socket
            m_acceptor.accept(*socket);
            // Send the socket to the handler
            m_callback(socket);
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}