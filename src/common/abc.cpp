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
	std::string jsondata = "{\"name\":\"bibek\",\"age\":22}";
	httpHandler.SendPostRequest("/", jsondata);
	httpHandler.GetResponse();
	}
	catch(Exception &ex)
	{
		std::cout << ex.what();
	}
	catch(int a)
	{
		std::cout << a;
	}
	
	return 0;
}

