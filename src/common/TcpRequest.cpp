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

void TcpRequest::JoinChat(TcpHandler &tcpHandler, uint32_t groupId)
{
    m_document = Document();    // New Document
    /*
    Example:
        {
            Request-Type: 1
            Group-Id: 20
        }
    */
    m_document.SetObject();     
    m_document.AddMember("Request-Type", Value((int)JOIN_CHAT), m_document.GetAllocator());
    m_document.AddMember("Group-Id", Value(groupId), m_document.GetAllocator());
    
    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
}

void TcpRequest::ChatMessage(TcpHandler &tcpHandler, uint32_t messageSize, uint32_t groupId)
{
    m_document = Document();    // New Document
    /*
    Example:
        {
            Request-Type: 2
            Group-Id: 20
            Message-Size: 50
        }
    */
    m_document.SetObject();
    m_document.AddMember("Request-Type", Value((int)CHAT_MESSAGE), m_document.GetAllocator());
    m_document.AddMember("Group-Id", Value(groupId), m_document.GetAllocator());
    m_document.AddMember("Message-Size", Value(messageSize), m_document.GetAllocator());

    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
}

void TcpRequest::SendClientAddr(TcpHandler &tcpHandler, uint32_t clientId)
{
    m_document = Document();    // New Document
    /*
    Example:
        {
            Request-Type: 3
            Client-Id: 5
        }
    */
    m_document.SetObject();
    m_document.AddMember("Request-Type", Value((int)SEND_CLIENT_ADDR), m_document.GetAllocator());
    m_document.AddMember("Client-Id", Value(clientId), m_document.GetAllocator());

    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
}

void TcpRequest::ReceiveClientAddr(TcpHandler &tcpHandler, uint32_t clientId, const std::string &ip, uint16_t port)
{
    m_document = Document();    // New Document
    /*
    Example:
        {
            Request-Type: 4
            Client-Id: 5
            IP: 23.123.2.3
            Port: 23456
        }
    */
    m_document.SetObject();
    m_document.AddMember("Request-Type", Value((int)RECEIVE_CLIENT_ADDR), m_document.GetAllocator());
    m_document.AddMember("Client-Id", Value(clientId), m_document.GetAllocator());
    m_document.AddMember("IP", Value(ip.c_str(), m_document.GetAllocator()), m_document.GetAllocator());
    m_document.AddMember("Port", Value((unsigned int)port), m_document.GetAllocator());

    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
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

uint16_t TcpRequest::GetPort()
{
    return GetValue("Port").GetUint();
}

uint32_t TcpRequest::GetClientId()
{
    return GetValue("Client-Id").GetUint();
}

std::string TcpRequest::GetIp()
{
    return GetValue("IP").GetString();
}