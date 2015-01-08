#pragma once

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <common/common.h>


using namespace rapidjson;

// the request action codes
enum class REQUEST 
{
	EMPTY, // for notifying the server online status
	SIGNUP,
	LOGIN,
	GET_SERVER_LIST,
	JOIN_SERVER,
	QUERY_FRIENDS,
	QUERY_ONLINE_FRIENDS,
	FRIEND_REQUEST,
	SEND_MESSAGE,
	RECEIVE_OFF_MESSAGES,
	RECEIVE_FRIEND_REQUESTS,
	ACCEPT_FRIEND_REQUEST,
	SEARCH_PEOPLE,
	USER_DETAILS,
	LOGOUT,
	// USAGE : REQUEST req = REQUEST::FRIEND_REQUEST;
	// NOTE : when to obtain int value, static cast is to be used
};

// error codes
enum class ERROR
{
	ALL_OKAY,
	LOGIN_ERROR,
	SIGNUP_ERROR,
	INVALID_REQUEST,
	JSON_FIELD_ERROR,
	SERVER_ERROR,
	// USAGE : ERROR_CODE error = ERROR_CODE::LOGIN_ERROR
	// static cast for retrieving int value
};

// the response action codes
enum class RESPONSE
{
	// to be added
};

class JSONHandlerException : public Exception
{
	public:
		JSONHandlerException(const std::string & errorString) : 
			Exception("JSON Handler Error: " + errorString)
	{}
};


class JSONHandler
{
	public:
		JSONHandler();
		~JSONHandler();
		
		// SetRequest() is a function that sets basic json fields
		void SetRequest(REQUEST);
		void SetRequest(REQUEST, std::string user);
		// signup request creator
		void SetSignupRequest(std::string, std::string, std::string, std::string, std::string, std::string, std::string);
		// login request creator
		void SetLoginRequest(std::string username, std::string password);
		// joinserver request creater
		void SetJoinServerRequest(std::string serveName);
		// sendMessage request creator
		void SetSendMessageRequest(std::string username, std::string message);
		// return the string form of the json request
		std::string GetRequest();

		
		// get the response string and parse it
		void ParseResponse(std::string);

	private:
		// attributes
		Document m_request;
		Document m_response;
		std::string m_username;
		std::string m_sessionId;

		// shortcut for creating new json object
		inline void New()
		{
			m_request = Document();
			m_request.SetObject();
		}

};
