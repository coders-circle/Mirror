#include <common/common.h>
#include <common/TcpRequest.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

// Requests are of fixed size (defined by REQUEST_MAX_SIZE)
// This makes it simpler to receive just the required request data
//  from the socket and neither more nor less
const size_t REQUEST_MAX_SIZE = 150;

TcpRequest::TcpRequest()
{}

TcpRequest::~TcpRequest()
{}

std::string TcpRequest::GetJsonString()
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    m_document.Accept(writer);
    return buffer.GetString();
}

void TcpRequest::New()
{
    m_document = Document();
    m_document.SetObject();
}

void TcpRequest::Send(TcpHandler& tcpHandler)
{
    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
}

void TcpRequest::JoinChat(TcpHandler &tcpHandler, uint32_t groupId)
{
    /*
    Example:
        {
            Request-Type: 1
            Group-Id: 20
        }
    */  
    New();
    m_document.AddMember("Request-Type", Value((int)JOIN_CHAT), m_document.GetAllocator());
    m_document.AddMember("Group-Id", Value(groupId), m_document.GetAllocator());
    Send(tcpHandler);
}

void TcpRequest::ChatMessage(TcpHandler &tcpHandler, uint32_t messageSize, const std::string &userId, uint32_t groupId)
{
    /*
    Example:
        {
            Request-Type: 2
            User-Id: bibek@dahal.com
            Group-Id: 20
            Message-Size: 50
        }
    */
    New();
    m_document.AddMember("Request-Type", Value((int)CHAT_MESSAGE), m_document.GetAllocator());
    m_document.AddMember("User-Id", Value(userId.c_str(), m_document.GetAllocator()), m_document.GetAllocator());
    m_document.AddMember("Group-Id", Value(groupId), m_document.GetAllocator());
    m_document.AddMember("Message-Size", Value(messageSize), m_document.GetAllocator());
    Send(tcpHandler);;
}

void TcpRequest::P2PTcp(TcpHandler& tcpHandler, uint32_t clientId, const std::string &privateIp, uint16_t privatePort, const std::string &publicIp, uint16_t publicPort)
{
    /*
    Example:
        {
            Request-Type: 3
            Client-Id: 32
            Private-IP: 1.2.3.4
            Private-Port: 1234
            Public-IP: 2.3.5.6
            Public-Port: 2356
        }
    */
    New();
    m_document.AddMember("Request-Type", Value((int)P2P_TCP), m_document.GetAllocator());
    m_document.AddMember("Client-Id", Value(clientId), m_document.GetAllocator());
    m_document.AddMember("Private-IP", Value(privateIp.c_str(), m_document.GetAllocator()), m_document.GetAllocator());
    m_document.AddMember("Private-Port", Value((unsigned int)privatePort), m_document.GetAllocator());
    m_document.AddMember("Public-IP", Value(publicIp.c_str(), m_document.GetAllocator()), m_document.GetAllocator());
    m_document.AddMember("Public-Port", Value((unsigned int)publicPort), m_document.GetAllocator());
    Send(tcpHandler);
}

void TcpRequest::Disconnect(TcpHandler &tcpHandler)
{
    /*
    Example:
        {
            Request-Type: 4
        }
    */
    New();
    m_document.AddMember("Request-Type", Value((int)DISCONNECT), m_document.GetAllocator());
    Send(tcpHandler);
}

void TcpRequest::UdpPort(TcpHandler& tcpHandler, uint16_t port)
{
    /*
    Example:
        {
            Request-Type: 5
            Udp-Port: 1234
        }
    */
    New();
    m_document.AddMember("Request-Type", Value((int)UDP_PORT), m_document.GetAllocator());
    m_document.AddMember("Udp-Port", Value((unsigned int)port), m_document.GetAllocator());
    Send(tcpHandler);
}

void TcpRequest::Invalid(TcpHandler& tcpHandler)
{
    /*
    Example:
        {
            Request-Type: 0
        }
    */
    New();
    m_document.AddMember("Request-Type", Value((int)INAVLID_TYPE), m_document.GetAllocator());
    Send(tcpHandler);
}

void TcpRequest::ReceiveRequest(TcpHandler &tcpHandler)
{
    // Get the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    tcpHandler.Receive(request, REQUEST_MAX_SIZE);

    // Parse the request
    m_document = Document();
    m_document.Parse(request);
}

TcpRequest::REQUEST_TYPE TcpRequest::GetRequestType()
{
    auto value = m_document.FindMember("Request-Type");
    if (value == m_document.MemberEnd() || !value->value.IsInt())
        return INAVLID_TYPE;
    return (REQUEST_TYPE)value->value.GetInt();
}

inline Value& TcpRequest::GetValue(const std::string &key)
{
    static Value sval;
    auto value = m_document.FindMember(key.c_str());
    if (value == m_document.MemberEnd())
        throw RequestException("Invalid value for '" + key + "' received in the request");
    sval = value->value;
    return sval;
}

uint32_t TcpRequest::GetGroupId()
{
    return GetValue("Group-Id").GetUint();
}

uint32_t TcpRequest::GetMessageSize()
{
    return GetValue("Message-Size").GetUint();;
}

uint32_t TcpRequest::GetClientId()
{
    return GetValue("Client-Id").GetUint();
}

std::string TcpRequest::GetPrivateIp()
{
    return GetValue("Private-IP").GetString();
}

uint16_t TcpRequest::GetPrivatePort()
{
    return GetValue("Private-Port").GetUint();
}

std::string TcpRequest::GetPublicIp()
{
    return GetValue("Public-IP").GetString();
}

uint16_t TcpRequest::GetPublicPort()
{
    return GetValue("Public-Port").GetUint();
}

std::string TcpRequest::GetUserId()
{
    return GetValue("User-Id").GetString();
}

uint16_t TcpRequest::GetUdpPort()
{
    return GetValue("Udp-Port").GetUint();
}
