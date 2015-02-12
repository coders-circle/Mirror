#include <common/common.h>
#include <client/Client.h>
#include <common/ChatMessage.h>

Client::Client()
: m_udpHandler1(m_io), m_udpHandler2(m_io)
{
    // Get a pair of consecutive even and odd udp ports
    UdpHandler::GetUdpPairs(m_udpHandler1, m_udpHandler2);
    std::cout << "Udp Ports Used: \n" << m_udpHandler1.GetSocket()->local_endpoint().port() 
                << ", " << m_udpHandler2.GetSocket()->local_endpoint().port() << std::endl;

    m_rtpStreamer.Initialize(&m_udpHandler1);
}

Client::~Client()
{}

void Client::SetServer(size_t connectionId)
{ 
    m_serverId = connectionId;
    m_request.ClientId(m_connections[m_serverId].tcpHandler);
    m_request.ReceiveRequest(m_connections[m_serverId].tcpHandler);
    if (m_request.GetRequestType() != TcpRequest::CLIENT_ID)
        throw Exception("Can't find client id");
    m_clientId = m_request.GetClientId();
    std::cout << "This client's Id: " << m_clientId << std::endl;
}

// Connecting to some remote peer (server/client)
size_t Client::Connect(const tcp::endpoint& peer, uint32_t secondsToWait)
{
    // Lock the connections list not to process it while adding a connection
    boost::lock_guard<boost::mutex> guard(m_mutex);

    // Add new connection
    m_connections.push_back(Connection(m_io));
    size_t cid = m_connections.size() - 1;
    TcpHandler &handler = m_connections[cid].tcpHandler;
    m_connections[cid].connected = false;
    
    // Set timeout for connection
    // After the timeout, the tcp socket is cancelled and closed;
    // which will throw an exception and the function returns
    boost::thread timerThread(boost::bind(&Client::ConnectTimer, this, _1, _2), boost::ref(handler), secondsToWait);

    // Establish the tcp connection
    handler.Initialize(peer);
    m_connections[cid].connected = true;

    // Establish the udp connection; this requires sending the other peer/server 
    // a request to send its udp-port
    m_request.UdpPort(handler);
    m_request.ReceiveRequest(handler);
    if (m_request.GetRequestType() != TcpRequest::UDP_PORT)
        throw Exception("Couldn't connect udp");
    uint16_t up = m_request.GetUdpPort();
    m_connections[cid].udpEndpoint = udp::endpoint(peer.address(), up);
    // Send a byte to ensure the udp connection on other side
    char c = 0;
    m_udpHandler1.Send(m_connections[cid].udpEndpoint, &c, 1); 

    std::cout << "Connected to: " << handler.GetDestinationAddress() << std::endl;
    return m_connections.size() - 1;
}

void Client::ConnectTimer(TcpHandler &handler, uint32_t seconds)
{
    boost::asio::deadline_timer t(m_io, boost::posix_time::seconds(seconds));
    t.wait();
    handler.Cancel();
    handler.Close();
}

// Perform above function asynchronously
void Client::ConnectAsync(const tcp::endpoint& peer, bool* threadEnd, size_t* connectionId, uint32_t secondsToWait)
{

    boost::thread t([this, peer, threadEnd, connectionId, secondsToWait](){
        try
        {
            if (threadEnd)
                *threadEnd = false;
            size_t id = Connect(peer, secondsToWait);
            if (connectionId)
                *connectionId = id;
            if (threadEnd)
                *threadEnd = true;
        }
        catch (std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
            if (threadEnd)
                *threadEnd = true;

        }
    });
}

void Client::HandleRequestsAsync() 
{ 
    boost::thread t(boost::bind(&Client::HandleRequests, this));
}

// An infinite loop to handle incoming requests
void Client::HandleRequests()
{
    while (true)
    {
        // Sleep for some time if there is not connection
        while (m_connections.size() == 0)
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

        // While processing connections, lock the list so no new connection is added at the time
        m_mutex.lock();
        for (unsigned int i = 0; i < m_connections.size(); ++i)
        {
            if (!m_connections[i].connected) continue;
            try
            {
                // Sleep some time to save CPU usage
                boost::this_thread::sleep(boost::posix_time::milliseconds(200));
                // See if any request is incomming for this connection
                size_t bytes = m_connections[i].tcpHandler.Available();
                ChatMessage chat;
                // if so, process accordingly
                if (bytes > 0)
                {
                    m_request.ReceiveRequest(m_connections[i].tcpHandler);
                    switch (m_request.GetRequestType())
                    {
                    // Request to receive an incoming chat message
                    case TcpRequest::CHAT_MESSAGE:
                        chat.Receive(m_connections[i].tcpHandler, m_request.GetMessageSize());
                        if (m_messageHandler)
                        {
                            boost::shared_ptr<MessageEventData> msgData(new MessageEventData);
                            msgData->senderId = m_request.GetUserId();
                            msgData->connectionId = i;
                            msgData->message = chat.GetMessage();
                            m_messageHandler(msgData);
                        }
                        break;
                    default:
                        std::cout << "Invalid Request " << m_request.GetRequestType() << std::endl;
                    }
                }
            }
            catch (std::exception &ex)
            {
                m_connections[i].tcpHandler.Close();
                m_connections[i].connected = false;
                std::cout << ex.what() << std::endl;
            }
        }
        m_mutex.unlock();
    }
}

void Client::Disconnect(size_t connectionId)
{
    m_request.Disconnect(m_connections[connectionId].tcpHandler);
    m_connections[connectionId].tcpHandler.Close();
    m_connections[connectionId].connected = false;
}

bool Client::IsConnected(size_t connectionId)
{
    return m_connections[connectionId].connected;
}

bool Client::JoinChat(uint32_t connectionId, uint32_t groupId)
{
    m_request.JoinChat(m_connections[connectionId].tcpHandler, groupId);
    // We need a JOIN_CHAT request back from other peer/server to ensure chat is joined
    m_request.ReceiveRequest(m_connections[connectionId].tcpHandler);
    if (m_request.GetRequestType() != TcpRequest::JOIN_CHAT)
        return false;
    return true;
}

bool Client::JoinVideoChat(uint32_t connectionId, uint32_t groupId)
{
    m_request.JoinVideoChat(m_connections[connectionId].tcpHandler, groupId);
    // We need a JOIN_VIDEO_CHAT request back from other peer/server to ensure chat is joined
    m_request.ReceiveRequest(m_connections[connectionId].tcpHandler);
    if (m_request.GetRequestType() != TcpRequest::JOIN_VIDEO_CHAT)
        return false;
    return true;
}

void Client::SendMessage(size_t receiverId, const std::string& message, uint32_t groupId)
{
    if (receiverId >= m_connections.size())
        throw Exception("Invalid Connection-Id to send message to");
    // Send the chat message after a CHAT_MESSAGE request
    m_request.ChatMessage(m_connections[receiverId].tcpHandler, message.size() + 1, m_name, groupId);
    ChatMessage chat;
    chat.SetMessage(message);
    chat.Send(m_connections[receiverId].tcpHandler);
}


void Client::TestVideo()
{
    boost::thread t([this](){m_rtpStreamer.StartReceiving();});
    RtpPacket packet;
    packet.Initialize(&m_udpHandler1, m_connections[m_serverId].udpEndpoint);
    packet.SetSourceId(m_clientId);
    int i=0;
    while(1)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(200));
        if (i<10){
        std::vector<char> data;
        data.resize(50000);
        for (size_t i=0; i<data.size(); ++i)
            data[i] = i%255;
        m_rtpStreamer.Send(packet, (uint8_t*)&data[0], data.size());
        std::cout << "Sending: " << data.size() << std::endl;
        i++;
        }
        uint8_t * rdata;
        if (m_rtpStreamer.GetSources().size() > 0)
        {
            uint32_t srcid = m_rtpStreamer.GetSources()[0];
            size_t len = m_rtpStreamer.GetPacket(srcid, &rdata, malloc);
            std::cout << len << " from " << srcid << std::endl;
        }
    }
    m_rtpStreamer.StopReceiving();
}

