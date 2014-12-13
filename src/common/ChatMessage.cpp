#include <common/common.h>
#include <common/ChatMessage.h>

ChatMessage::ChatMessage()
{}

ChatMessage::~ChatMessage()
{}

struct ChatHeader
{
    size_t body_size;
};

void ChatMessage::Send(TcpHandler &tcpHandler)
{
    ChatHeader header;
    header.body_size = m_body.size() + 1;           // add 1 to size to ensure null terminator
    tcpHandler.Send((char*)&header, sizeof(ChatHeader));
    tcpHandler.Send(m_body.c_str(), header.body_size); 
}

void ChatMessage::Receive(TcpHandler &tcpHandler)
{
    ChatHeader header;
    tcpHandler.Receive((char*)&header, sizeof(ChatHeader));
    char * data = new char[header.body_size];
    tcpHandler.Receive(data, header.body_size);
    m_body = std::string(data);
    delete[] data;
}