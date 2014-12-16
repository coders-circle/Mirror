#include <common/common.h>
#include <common/RequestHandler.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

// Requests are of fixed size (defined by REQUEST_MAX_SIZE)
// This makes it simpler to receive just the required request data
//  from the socket and neither more nor less
const size_t REQUEST_MAX_SIZE = 150;

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}

std::string RequestHandler::GetJsonString()
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    m_document.Accept(writer);
    return buffer.GetString();
}

void RequestHandler::JoinGroup(TcpHandler &tcpHandler, uint32_t groupId)
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
    m_document.AddMember("Request-Type", Value((int)JOIN_GROUP), m_document.GetAllocator());
    m_document.AddMember("Group-Id", Value(groupId), m_document.GetAllocator());
    
    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy_s(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
}

void RequestHandler::GroupChat(TcpHandler &tcpHandler, uint32_t groupId)
{
    m_document = Document();    // New Document
    /*
    Example:
        {
            Request-Type: 2
            Group-Id: 20
        }
    */
    m_document.SetObject();
    m_document.AddMember("Request-Type", Value((int)GROUP_CHAT), m_document.GetAllocator());
    m_document.AddMember("Group-Id", Value(groupId), m_document.GetAllocator());

    // Send the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    strcpy_s(request, GetJsonString().c_str());
    tcpHandler.Send(request, REQUEST_MAX_SIZE);
}

void RequestHandler::ReceiveRequest(TcpHandler &tcpHandler)
{
    // Get the "fixed-size" request
    char request[REQUEST_MAX_SIZE];
    tcpHandler.Receive(request, REQUEST_MAX_SIZE);

    // Parse the request
    m_document = Document();
    m_document.Parse(request);
}

RequestHandler::REQUEST_TYPE RequestHandler::GetRequestType()
{
    // Return INVALID_TYPE when Request-Type value is absent or isn't integer
    if (!m_document.HasMember("Request-Type") || !m_document["Request-Type"].IsInt())
        return INAVLID_TYPE;
    return (REQUEST_TYPE)m_document["Request-Type"].GetInt();
}

uint32_t RequestHandler::GetGroupId()
{
    // Throw exception for absence of or invalid group-id
    if (!m_document.HasMember("Group-Id") || !m_document["Group-Id"].IsInt())
        throw RequestException("Invalid group-id received");
    return m_document["Group-Id"].GetUint();
}