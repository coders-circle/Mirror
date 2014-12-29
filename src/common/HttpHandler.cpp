#include <common/common.h>
#include "common/HttpHandler.h"

// the construtor, takes reference to io_service as argument
HttpHandler::HttpHandler(boost::asio::io_service& ioService):m_ioService(ioService), TcpHandler(ioService)
{
}

// the destrutor
HttpHandler::~HttpHandler()
{
}

void HttpHandler:: SendGetRequest(std::string location)
{
	// create a GET request
	std::string requestStr = "GET " + location + " HTTP/1.1\r\n";
	requestStr += "Host: " + m_hostName + "\r\n";
	requestStr += "Accept: */*\r\n";
	requestStr += "Connection: Keep-Alive\r\n\r\n";

	// convert requestStr to char*
	int length  = requestStr.length() + 1;
	// first create a temporary char* 
	char *temp = new char[length];
	strcpy(temp, requestStr.c_str());
	// convert it to const char* bacause base class's Send requires const char*
	const char*getRequest = (const char*)temp;

	// send through base class's send method
	TcpHandler::Send(getRequest, length);
}


boost::shared_ptr<tcp::socket> HttpHandler::CreateSocket(boost::asio::io_service &ioService, std::string hostName, std::string port)
{
	tcp::resolver resolver(ioService);
	tcp::resolver::query query(hostName, port);
	tcp::resolver::iterator endpt_iterator = resolver.resolve(query);
	tcp::resolver::iterator end;

	//try each endpoint until we successfully establish a connection
	tcp::socket socket(ioService);
	boost::system::error_code error  = boost::asio::error::host_not_found;
	
	while (error && endpt_iterator != end)
	{
		socket.close();
		socket.connect(*endpt_iterator++, error);
	}
	if (error)
	{
		throw boost::system::system_error(error);
	}
	boost::shared_ptr<tcp::socket> sptr(&socket);
	return sptr;
}
// the initialize function
//	takes hostName and port as string, sets the member variables
//	m_hostName and m_port, creates socket and calls the Initialize meth//	method of base class
void HttpHandler::Initialize(std::string hostName, std::string port)
{
	m_hostName = hostName;
	m_port = port;
	// creating a socket ptr using static method CreateSocket
	boost::shared_ptr<tcp::socket> socket = CreateSocket(m_ioService, m_hostName, m_port);
	// calling the base class method Initialize
	TcpHandler::Initialize(socket);
}


