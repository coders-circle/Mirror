#pragma once

//don't use pragma, use standard header guard technique

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

		// initialize with a socket
		void Initialize(boost::shared_ptr<tcp::socket> socket);
		// send tcp request to the endpoint and create socket
		void Initialize(const tcp::endpoint &destEndpoint);

		void Send(const char* data, size_t size);
		void Receive(char* data, size_t max_size);
        size_t Available();

        std::string GetDestinationAddress() const;

	private:
		boost::shared_ptr<tcp::socket> m_socket;
		boost::asio::io_service &m_ioService;
};
