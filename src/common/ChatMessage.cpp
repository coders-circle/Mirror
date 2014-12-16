#include <common/common.h>
#include <common/ChatMessage.h>

ChatMessage::ChatMessage()
{}

ChatMessage::~ChatMessage()
{}

void ChatMessage::Send(TcpHandler &tcpHandler)
{
    uint32_t body_size = m_body.size() + 1;
    tcpHandler.Send((char*)&body_size, sizeof(body_size));
    tcpHandler.Send(m_body.c_str(), body_size); 
}

void ChatMessage::Receive(TcpHandler &tcpHandler)
{
    uint32_t body_size;
    tcpHandler.Receive((char*)&body_size, sizeof(body_size));
    char * data = new char[body_size];
    tcpHandler.Receive(data, body_size);
    m_body = std::string(data);
    delete[] data;
}