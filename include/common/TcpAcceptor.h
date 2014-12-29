#pragma once

class TcpAcceptor
{
public:
    TcpAcceptor(boost::asio::io_service &io);
    ~TcpAcceptor();

    // Initialize the acceptor to listen at given local endpoint
    uint16_t Initialize(const tcp::endpoint &localEndpoint);
    // Listen for incomming connections in a new thread 
    //  and call 'callback' for each connection
    void Listen(boost::function<void(boost::shared_ptr<tcp::socket>)> callback);

    tcp::acceptor &GetSocket() { return m_acceptor; }

private:
    tcp::acceptor m_acceptor;
    boost::function<void(boost::shared_ptr<tcp::socket>)> m_callback;

    void StartListening();
    void ListeningThread();
};