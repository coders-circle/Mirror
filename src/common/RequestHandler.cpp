#include <common/common.h>
#include <common/RequestHandler.h>

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}

void RequestHandler::JoinGroup(TcpHandler &tcpHandler, uint32_t groupId)
{
    m_request.type = JOIN_GROUP;
    m_request.info.join.groupId = groupId;
    tcpHandler.Send((char*)&m_request, sizeof(m_request));
}

void RequestHandler::GroupChat(TcpHandler &tcpHandler, uint32_t groupId)
{
    m_request.type = GROUP_CHAT;
    m_request.info.groupChat.groupId = groupId;
    tcpHandler.Send((char*)&m_request, sizeof(m_request));;
}

void RequestHandler::ReceiveRequest(TcpHandler &tcpHandler)
{
    tcpHandler.Receive((char*)&m_request, sizeof(m_request));
}