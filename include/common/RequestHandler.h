#pragma once
#include "TcpHandler.h"

const size_t REQUEST_MAX_SIZE = 150;
class RequestHandler
{
public:
    enum REQUEST_TYPE{ JOIN_GROUP = 0 };
    struct Request
    {
        REQUEST_TYPE type;
        union
        {
            struct
            {
                uint32_t groupId;
            } join;
        } info;
    };

    RequestHandler();
    ~RequestHandler();

    static void JoinGroup(TcpHandler &tcpHandler, uint32_t groupId);

    void ReceiveRequest(TcpHandler &tcpHandler);
    const Request& GetLastRequest() { return m_request; }
private:
    Request m_request;
};
