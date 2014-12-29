#include <common/common.h>
#include <common/HttpHandler.h>

int main()
{
	boost::asio::io_service io_service;
	try
	{
	HttpHandler httpHandler(io_service);
	
	std::string host = "localhost";
	std::string port = "8000";
	
	httpHandler.Initialize(host, port);
	std::string json = "{\"username\" : \"bpandey\",\"password\" : \"mysql\"}";
	httpHandler.SendPostRequest("/test/process-login/", json);
	httpHandler.GetResponse();
	}
	catch(int a)
	{
		std::cout << a;
	}
	
	return 0;
}

