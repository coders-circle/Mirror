#pragma once
#include "TcpHandler.h"

const size_t REQUEST_MAX_SIZE = 150;
class RequestHandler
{
public:
    enum REQUEST_TYPE{ JOIN_GROUP = 0, GROUP_CHAT };
    struct Request
    {
        REQUEST_TYPE type;  // Request type
        union               // Union of request data for each type
        {
            // JOIN_GROUP
            struct
            {
                uint32_t groupId;   // group-id to join
            } join;
            // GROUP_CHAT
            struct
            {
                uint32_t groupId;   // group-id to send chat message to
            } groupChat;
        } info;
    };

    RequestHandler();
    ~RequestHandler();

    // Request the server to join a group
    void JoinGroup(TcpHandler &tcpHandler, uint32_t groupId);
    // Request the server to wait for incoming chat message for a group
    void GroupChat(TcpHandler &tcpHandler, uint32_t groupId);
    
    
    // Receive a request 
    void ReceiveRequest(TcpHandler &tcpHandler);
    // Get last sent/received request
    const Request& GetLastRequest() { return m_request; }
private:
    Request m_request;
};