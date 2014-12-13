#include <common/common.h>
#include <common/TcpListener.h>

TcpListener::TcpListener(boost::asio::io_service &io) : m_acceptor(io)
{}

TcpListener::~TcpListener()
{}

void TcpListener::Initialize(const tcp::endpoint &localEndpoint)
{
    m_acceptor.open(localEndpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(localEndpoint);
    m_acceptor.listen();
}

void ListeningThread()
{

}

void TcpListener::Listen(boost::function<void(boost::shared_ptr<tcp::socket>)> callback)
{
    m_callback = callback;
    StartListening();
}

void TcpListener::StartListening()
{
    boost::thread t(boost::bind(&TcpListener::NewThread, this));
}

void TcpListener::NewThread()
{
    while (true)
    {
        boost::shared_ptr<tcp::socket> socket(new tcp::socket(m_acceptor.get_io_service()));
        m_acceptor.accept(*socket);
        m_callback(socket);
    }
}