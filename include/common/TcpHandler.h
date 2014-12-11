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
		TcpHandler();
		~Tcphandler();

		// initialize with a socket
		Initialize(boost::shared_ptr<tcp::socket> socket);
		// send tcp request to the endpoint and create socket
		Initialize(tcp::endpoint &endpoint); 

		// initial request for creating connection
		Request(tcp::endpoint &endpoint);

		Send(const std::vector<char> & data);
		
		Receive(const std::vector<char> & data);

	private:
		boost::shared_ptr<tcp::socket> m_socket;
		boost::shared_ptr<tcp::endpoint> m_destination;
		boost::shared_ptr<TcpTransmitter> m_transmitter;
		boost::shared_ptr<TcpReceiver> m_receiver;
		const int m_port;
};
