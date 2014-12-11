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

    void Initialize(const tcp::endpoint &localEndpoint);
    void Listen(boost::function<void(boost::shared_ptr<tcp::socket>)> callback);

private:
    tcp::acceptor m_acceptor;
    boost::function<void(boost::shared_ptr<tcp::socket>)> m_callback;

    void StartListening();
    void HandleAccept(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
};