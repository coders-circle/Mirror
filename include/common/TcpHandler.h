#pragma once

class TcpHandlerException : public Exception
{
	public:
		TcpHandlerException(const std::string &errorString) : 
			Exception("TCP Handler Error: " + errorString)
	{}
};


class TcpHandler
{
public:
	TcpHandler(boost::asio::io_service &ioService);
    ~TcpHandler();

	// Initialize with an already created socket
	void Initialize(boost::shared_ptr<tcp::socket> socket);
	// Connect to given endpoint and initialize by creating new socket
	void Initialize(const tcp::endpoint &destEndpoint);

    // Send data of given size
	void Send(const char* data, size_t size);
    // Receive data of given size
	void Receive(char* data, size_t size);
    // Receive data of given size or less
    size_t ReceiveSome(char* data, size_t max_size);
    // Check if the socket has data to be read
    size_t Available();

    std::string GetDestinationAddress() const;
    std::string GetRemoteIp() const { return m_socket->remote_endpoint().address().to_string(); }
    uint16_t GetRemotePort() const { return m_socket->remote_endpoint().port(); }
    boost::shared_ptr<tcp::socket> GetSocket() const { return m_socket; }

protected:
	boost::shared_ptr<tcp::socket> m_socket;
	boost::asio::io_service &m_ioService;
};
