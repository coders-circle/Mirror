#include <common/common.h>
#include <common/HttpHandler.h>
#include <common/JSONHandler.h>

int main()
{
	boost::asio::io_service io_service;
	try
	{
	JSONHandler jsonhandler;
	jsonhandler.SetSignupRequest("bpandey", "mysql","bewakepandey@gmail.com",  "Bibek", "Pandey", "nepal", "1994/12/29");
	jsonhandler.SetLoginRequest("bibek", "mysql");
	std::string host = "localhost";
	std::string port = "8000";
	
	HttpHandler* httpHandler;
	std::string response;
	while(1)
	{
	httpHandler = new HttpHandler(io_service);
	(*httpHandler).Initialize(host, port);
	(*httpHandler).SendPostRequest("/", "my name is bibek");
	//httpHandler.SendGetRequest(jsonhandler.GetRequest());
	response = (*httpHandler).GetResponse();
	jsonhandler.ParseResponse(response);
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	delete httpHandler;
	}
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

