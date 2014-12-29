#include <common/common.h>
#include <common/ChatMessage.h>

ChatMessage::ChatMessage()
{}

ChatMessage::~ChatMessage()
{}

void ChatMessage::Send(TcpHandler &tcpHandler)
{
    tcpHandler.Send(m_body.c_str(), m_body.size() + 1);
}

void ChatMessage::Receive(TcpHandler &tcpHandler, uint32_t size)
{
    char * data = new char[size];
    tcpHandler.Receive(data, size);
    m_body = std::string(data);
    delete[] data;
}
