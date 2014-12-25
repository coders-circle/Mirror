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
class TcpRequest
{
public:
    enum REQUEST_TYPE{ 
        INAVLID_TYPE = 0, 
        JOIN_CHAT, CHAT_MESSAGE, 
        SEND_CLIENT_ADDR, RECEIVE_CLIENT_ADDR,
    };

    TcpRequest();
    ~TcpRequest();

    // Request to join a chat
    void JoinChat(TcpHandler &tcpHandler, uint32_t groupId = 0);
    // Request to wait for incoming chat message
    void ChatMessage(TcpHandler &tcpHandler, uint32_t messageSize, uint32_t groupId = 0);
    // Request to send a client address (ip and port)
    // For server, provide a valid clientId
    void SendClientAddr(TcpHandler &tcpHandler, uint32_t clientId);
    // Request to receive a client address (ip and port) as response for a SEND_CLIENT_ADDR request
    // For server, provide a valid clientId
    void ReceiveClientAddr(TcpHandler &tcpHandler, uint32_t clientId, const std::string &ip, uint16_t port);
    // Receive a request 
    void ReceiveRequest(TcpHandler &tcpHandler);

    // Request Data
    REQUEST_TYPE GetRequestType();
    uint32_t GetGroupId();
    uint32_t GetMessageSize();
    uint32_t GetClientId();
    std::string GetIp();
    uint16_t GetPort();
private:
    // The Json Document that holds the request
    rapidjson::Document m_document;
    // Get string from the Json Doucment
    std::string GetJsonString();
    // Helper function get a json value from a request
    inline rapidjson::Value& GetValue(const std::string &key);
};
