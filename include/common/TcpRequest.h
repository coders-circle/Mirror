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
        P2P_TCP,
    };

    TcpRequest();
    ~TcpRequest();

    // Request to join a chat
    void JoinChat(TcpHandler &tcpHandler, uint32_t groupId = 0);
    // Request to wait for incoming chat message
    void ChatMessage(TcpHandler &tcpHandler, uint32_t messageSize, const std::string &userId, uint32_t groupId = 0);
    // Request to establish a P2P TCP connection with a client
    // Contains private and public addess-port pairs and client-id
    void P2PTcp(TcpHandler& tcpHandler, uint32_t clientId, const std::string &privateIp, uint16_t privatePort, const std::string &publicIp = "", uint16_t publicPort = 0);
    // Send an invalid request
    void Invalid(TcpHandler& tcpHandler);

    // Receive a request 
    void ReceiveRequest(TcpHandler &tcpHandler);

    // Request Data
    REQUEST_TYPE GetRequestType();
    uint32_t GetGroupId();
    uint32_t GetMessageSize();
    uint32_t GetClientId();
    std::string GetPrivateIp();
    uint16_t GetPrivatePort();
    std::string GetPublicIp();
    uint16_t GetPublicPort();
    std::string GetUserId();
private:
    // The Json Document that holds the request
    rapidjson::Document m_document;
    // Get string from the Json Doucment
    std::string GetJsonString();
    // Helper function get a json value from a request
    inline rapidjson::Value& GetValue(const std::string &key);
    // Helper function to initialize new request
    inline void New();
    // Helper function to send the request
    inline void Send(TcpHandler& tcpHandler);
};
