#pragma once

class TcpReceiverException: public Exception
{
	public:
		TcpReceiverException(const std::string &errorString) : 
			Exception("TCP Receiver Error: " + errorString)
	{}
};

class TcpReceiver
{
	public:
		TcpReceiver();
		~TcpReceiver();

		void Initialize(boost::shared_ptr<tcp::socket> socket,
				const tcp::endpoint &destination);

		void Receive(std::vector<char> &data);

	private:
		boost::shared_ptr<tcp::socket> m_socket;
		tcp::endpoint m_destination;
};
