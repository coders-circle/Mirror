#pragma once

class TcpListenerException : public Exception
{
public:
    TcpListenerException(const std::string &errorString)
        : Exception("TCP Listener Error: " + errorString)
    {}
};

class TcpListener
{
public:
    TcpListener(boost::asio::io_service &io);
    ~TcpListener();

    // Initialize the listener to listen at given local endpoint
    void Initialize(const tcp::endpoint &localEndpoint);
    // Listen for incomming connections and call 'callback' when connected to any
    //  The callback is called on a new thread
    void Listen(boost::function<void(boost::shared_ptr<tcp::socket>)> callback);

private:
    tcp::acceptor m_acceptor;
    boost::function<void(boost::shared_ptr<tcp::socket>)> m_callback;

    void StartListening();
    void ListeningThread();
};