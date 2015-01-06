#pragma once
#include "rapidjson/docoument.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

// the request action codes
enum class REQUEST
{
	EMPTY, // for notifying the server online status
	SIGNUP,
	LOGIN,
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
		
		void Login(std::string username, std::string password);
		//void QueryFriends();
		//void QueryOnlineFriends();

		void CreateRequest(int actionCode);
		void CreateRequest(int actionCode, std::string user);
		/*void SendFriendRequest(std::string friendName);
		void SearchPeople(std::string name);
		void GetUserDetail(std::string name);
		void JoinServer(std::string serveName);
		*/

		void SignupRequest(/*Signup data */);

		/*void SendCallRequest(std::string username);
		void AcceptFriend(std::string username);
		void ReceiveOffMessages();
		void ReceiveFriendRequests();
		*/
		void SendMessageRequest(std::string username, std::string message);
/*
	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    m_document.Accept(writer);
    return buffer.GetString();
*/
	private:
		// attributes
		rapidjson::Document m_request;
		rapidjson::Document m_rsponse;
		std::string m_usename;
		std::string m_sessionId;

		// shortcut for creating new json object
		inline void New()
		{
			m_request = rapidjson::Document();
			m_request.SetObject();
		}

};
