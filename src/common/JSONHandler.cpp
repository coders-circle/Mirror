#include <common/common.h>
#include <common/JSONHandler.h>
using namespace rapidjson;

// constructor.
JSONHandler::JSONHandler()
{
	m_usename = "";
	m_sessionId = "";
}

JSONHandler::~JSONHandler()
{}

// requests without other parameters except actionCode
void JSONHandler::Request(int actionCode)
{
	// first initialize empty json document
	New();
	// add the action code
	m_request.AddMember("action", Value((int)/*Action Code*/), m_request.GetAllocator());
	// add the session ID field
	m_request.AddMember("sessionId", Value(m_sessionId.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	// add the username field
	m_request.AddMember("username", Value(m_username.c_str(), m_request.GetAllocator()), m_request.GetAllocator());

}


// common fields in the json request(usename and sessionId after login)
void JSONHandler::Request(int actionCode, std::string user)
{
	Request(actionCode);
	// add the other user field
	m_request.AddMember("user", Value(user.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// a login json request is created
void JSONHandler::Login(std::string username, std::string password)
{
	New();
	m_request.AddMember("username", Value(username.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("password", Value(password.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// request for sending message
void JSONHandler::SendMessage(std::string user, std::string message)
{
	// general request
	CreateRequest(/*actioncode*/, user);
	// add the message field
	m_request.AddMember("message", Value(message.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// request for joining server
void JSONHandler::JoinServer(std::string serverName)
{
	// general request
	CreateRequest(/*actioncode*/);
	// add the servername field
	m_request.AddMember("server", Value(serverName.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// request for signing up
void JSONHandler::Signup(/*Signup Data*/)
{
	New();
	m_request.AddMember("username", Value(username.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("password", Value(password.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("firstName", Value(firstName.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("lastName", Value(lastName.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("location", Value(location.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("DOB", Value(DOB.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// get the response in string format, this is passed to the HttpHandler
std::string JSONHandler::GetRequest()
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	m_request.Accept(writer);
	return buffer.GetString();
}
