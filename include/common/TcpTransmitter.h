#pragma once

class TcpTransmitterException: public Exception
{
	public:
		TcpTransmitterException(const std::string &errorString)
			: Exception("TCP Transmission Error: " + errorString)
		{}
};

class TcpTransmitter
{
	public:
		TcpTransmitter();
		~TcpTransmitter();

		void Initialize(boost::shared_ptr<tcp::socket> socket, const tcp::endpoint &destination);
        void Send(const std::vector<char> &data);

	private:
		boost::shared_ptr<tcp::socket> m_socket;
		tcp::endpoint m_destination;
};
