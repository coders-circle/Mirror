#pragma once
#include "common/TcpHandler.h"

class Exception;
using boost::asio::ip::tcp;

class HttpHandlerException : public Exception
{
	public:
		HttpHandlerException(const std::string &errorString) :
			Exception("Http Handler Error: " + errorString)
	{}
};


class HttpHandler : public TcpHandler
{
	public:
		HttpHandler(boost::asio::io_service& ioService);
		~HttpHandler();

		//static function to connect to the server, returns a socket(or pointer(shared))
		static boost::shared_ptr<tcp::socket> CreateSocket(boost::asio::io_service &ioService, std::string hostName, std::string port);
		// overloaded Initialize method, takes hostname and port as string
		void Initialize(std::string hostName, std::string port);
		// send the GET request
		void SendGetRequest(std::string location);
		// send the POST request
		void SendPostRequest(std::string location, std::string jsonString);
		// get the response
		std::string GetResponse();

	private:
		//boost::asio::io_service& m_ioService;
		//boost::shared_ptr<tcp::socket> m_socket;
		boost::asio::streambuf m_serverResponse;
		std::string m_hostName, m_port;
};

