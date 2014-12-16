#pragma once
#include "TcpHandler.h"
#include "rapidjson/document.h"

class RequestException : public Exception
{
public:
    RequestException(const std::string &errorString)
        :Exception("Error processing request: "+errorString)
    {}
};
class RequestHandler
{
public:
    enum REQUEST_TYPE{ INAVLID_TYPE = 0, JOIN_GROUP, GROUP_CHAT };

    RequestHandler();
    ~RequestHandler();

    // Request the server to join a group
    void JoinGroup(TcpHandler &tcpHandler, uint32_t groupId);
    // Request the server to wait for incoming chat message for a group
    void GroupChat(TcpHandler &tcpHandler, uint32_t groupId);
    // Receive a request 
    void ReceiveRequest(TcpHandler &tcpHandler);

    // Request Data
    REQUEST_TYPE GetRequestType();
    uint32_t GetGroupId();
private:
    // The Json Document that holds the request
    rapidjson::Document m_document;
    // Get string from the Json Doucment
    std::string GetJsonString();
};
