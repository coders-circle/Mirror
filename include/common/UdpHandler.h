#pragma once

class UdpHandlerException : public Exception
{
public:
    UdpHandlerException(const std::string &errorString) :
        Exception("UDP Handler Error: " + errorString)
    {}
};

class UdpHandler
{
public:
    UdpHandler(boost::asio::io_service &ioService);
    ~UdpHandler();

    // Initialize with an already created socket
    void Initialize(boost::shared_ptr<udp::socket> socket);
    // Initialize with a socket bound to given local endpoint
    void Initialize(const udp::endpoint &localEndpoint);

    // Send data to a remote endpoint
    void Send(const udp::endpoint &remoteEndpoint, const char* data, size_t size);
    // Receive data from a remote endpoint; the remoteEndpoint is automatically populated
    size_t Receive(udp::endpoint &remoteEndpoint, char* data, size_t maxSize);

private:
    boost::shared_ptr<udp::socket> m_socket;
    boost::asio::io_service &m_ioService;
};