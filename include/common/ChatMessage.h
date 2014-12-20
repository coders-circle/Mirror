#pragma once
#include "TcpHandler.h"

// This class is used to send and receive chat messages with header through a TcpHandler
class ChatMessage
{
public:
    ChatMessage();
    ~ChatMessage();

    const std::string& GetMessage() const { return m_body; }
    void SetMessage(const std::string& message) { m_body = message; }

    void Send(TcpHandler &tcpHandler);
    void Receive(TcpHandler &tcpHandler, uint32_t size);

private:;
    std::string m_body;
};