#include <common/JSONHandler.h>

// constructor.
JSONHandler::JSONHandler()
{
	m_username = "";
	m_sessionId = "";
}

JSONHandler::~JSONHandler()
{}

// requests without other parameters except actionCode
void JSONHandler::SetRequest(REQUEST action)
{
	// first initialize empty json document
	New();
	// add the action code
	m_request.AddMember("action", Value((int)action), m_request.GetAllocator());
	// add the session ID field
	m_request.AddMember("sessionId", Value(m_sessionId.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	// add the username field
	m_request.AddMember("username", Value(m_username.c_str(), m_request.GetAllocator()), m_request.GetAllocator());

}


// common fields in the json request(usename and sessionId after login)
void JSONHandler::SetRequest(REQUEST action, std::string user)
{
	SetRequest(action);
	// add the other user field
	m_request.AddMember("user", Value(user.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// a login json request is created
void JSONHandler::SetLoginRequest(std::string username, std::string password)
{
	New();
	m_request.AddMember("action", Value((int)REQUEST::LOGIN), m_request.GetAllocator());
	m_request.AddMember("username", Value(username.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("password", Value(password.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// request for sending message
void JSONHandler::SetSendMessageRequest(std::string user, std::string message)
{
	// general request
	SetRequest(REQUEST::SEND_MESSAGE, user);
	// add the message field
	m_request.AddMember("message", Value(message.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// request for joining server
void JSONHandler::SetJoinServerRequest(std::string serverName)
{
	// general request
	SetRequest(REQUEST::JOIN_SERVER);
	// add the servername field
	m_request.AddMember("server", Value(serverName.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// request for signing up
void JSONHandler::SetSignupRequest(std::string username, std::string password, std::string email, std::string firstName, std::string lastName, std::string location, std::string DOB)
{
	New();
	m_request.AddMember("action", Value((int)REQUEST::SIGNUP), m_request.GetAllocator());

	m_request.AddMember("username", Value(username.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("password", Value(password.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("email", Value(email.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("firstName", Value(firstName.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("lastName", Value(lastName.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("location", Value(location.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
	m_request.AddMember("DOB", Value(DOB.c_str(), m_request.GetAllocator()), m_request.GetAllocator());
}

// return the response in string format, this is passed to the HttpHandler
std::string JSONHandler::GetRequest()
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	m_request.Accept(writer);
	return buffer.GetString();
}

// get the response string and parse it
void JSONHandler::ParseResponse(std::string responseString)
{
	char* response = new char[responseString.length()+1];
	strcpy(response, responseString.c_str());
	m_response.Parse<0>(response);

	// check for errors
	if (m_response["error"].GetInt() != (int)ERROR::ALL_OKAY)
		//HandleError();
		std::cout << "Error happened!!";
	else // if ALL_OKAY
	{
		switch(m_response["action"].GetInt())
		{
		}
	}
}
