#ifndef TCP_CONNECTION
#define TCP_CONNECTION

using boost::asio::ip::tcp;

class TCPConnection
{
	public:
		TCPConnection(boost::asio::io_service&);
		// member functions

	private:
		tcp::endpoint m_clientEndpoint;
		tcp::endpoint m_serverEndpoint;
		

#endif 
