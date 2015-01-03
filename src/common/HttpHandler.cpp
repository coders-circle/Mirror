#include "common/common.h"
#include "common/HttpHandler.h"

// the construtor, takes reference to io_service as argument
HttpHandler::HttpHandler(boost::asio::io_service& ioService):TcpHandler(ioService)
{
}

// the destrutor
HttpHandler::~HttpHandler()
{
}

void HttpHandler:: SendGetRequest(std::string location)
{
	// create a GET request
	std::string requestStr = "GET " + location + " HTTP/1.0\r\n";
	requestStr += "Host: " + m_hostName + "\r\n";
	requestStr += "Accept: application/json\r\n";
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
	boost::shared_ptr<tcp::socket> sptr(new tcp::socket(ioService));
	boost::system::error_code error  = boost::asio::error::host_not_found;
	
	while (error && endpt_iterator != end)
	{
		(*sptr).close();
		(*sptr).connect(*endpt_iterator++, error);
	}
	if (error)
	{
		throw boost::system::system_error(error);
	}
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


// sending the post request
void HttpHandler::SendPostRequest(std::string location, std::string jsonString)
{
	std::string requestStr = "POST " + location + " HTTP/1.0\r\n";
	requestStr += "Host: " + m_hostName + "\r\n";
	requestStr += "Content-Type: application/json\r\n";
	requestStr += "Accept: application/json\r\n";
	requestStr += "Content-Length: " + std::to_string(jsonString.length()) + "\r\n";
	requestStr += "Connection: Keep-Alive\r\n\r\n";
	requestStr += jsonString;
	
	// convert requestStr to char*
	int length  = requestStr.length();
	// first create a temporary char* 
	char *temp = new char[length];
	strcpy(temp, requestStr.c_str());
	// convert it to const char* bacause base class's Send requires const char*
	const char*postRequest = (const char*)temp;

	// send through base class's send method
	TcpHandler::Send(postRequest, length);
	//std::cout << Available() << std::endl;
}


// response after a GET or POST
void HttpHandler::GetResponse()
{
	try
	{
		boost::asio::read_until(*m_socket, m_serverResponse, "<<>>"); 
	}
	catch (Exception &e)
	{
		boost::asio::read_until(*m_socket, m_serverResponse, "\r\n\r\n");
	}
	// following printing is just for the test purpose
	std::istream responseStream(&m_serverResponse);
	std::string test;
	while(!std::getline(responseStream, test).eof())
	{
		std::cout << test << std::endl;
	}
	std::getline(responseStream, test);
	std::cout << test << std::endl;
}
