#include <common/common.h>
#include <common/RequestHandler.h>

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}

void RequestHandler::JoinGroup(TcpHandler &tcpHandler, uint32_t groupId)
{
    std::string request;
    request = "JOIN_REQUEST";
    request += "\t" + std::to_string(groupId);
    tcpHandler.Send(request.c_str(), request.size() + 1);
}

void RequestHandler::ReceiveRequest(TcpHandler &tcpHandler)
{
    char data[REQUEST_MAX_SIZE];
    tcpHandler.Receive(data, REQUEST_MAX_SIZE);
    std::string request(data);
    if (request.substr(0, 12) == "JOIN_REQUEST")
    {
        m_request.type = JOIN_GROUP;
        m_request.info.join.groupId = std::stoul(request.substr(13).c_str());
    }
}