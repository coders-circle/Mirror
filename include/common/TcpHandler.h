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
		void Initialize(tcp::endpoint &destEndpoint); 

		// initial request for creating connection
		void Request(tcp::endpoint &destEndpoint);// for normal tcp connect request

		void Send(const std::vector<char> &data);
		
		void Receive(std::vector<char> &data);

	private:
		boost::shared_ptr<tcp::socket> m_socket;
		boost::shared_ptr<tcp::endpoint> m_destination;
		boost::asio::io_service &m_ioService;
		std::vector<char> m_lastSent; // may not be needed
		std::vector<char> m_lastReceived; // may not be needed
};
