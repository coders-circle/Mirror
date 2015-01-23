#pragma once
#include <common/TcpHandler.h>
#include <common/UdpHandler.h>
#include <common/TcpRequest.h>
#include "MediaStream/VideoPlayback.h"
#include "MediaStream/VideoCapture.h"
#include "MediaStream/RtpStreamer.h"

/* Data passed to the message event handler */
struct MessageEventData
{
    size_t connectionId;
    std::string senderId;
    std::string message;
};

/* Class to handle networking connections in client application */
class Client
{
public:
    Client();
    ~Client();

    // Boost IO service
    boost::asio::io_service& GetIOService() { return m_io; }

    // Connect to a server,
    size_t Connect(const tcp::endpoint& peer, uint32_t secondsToWait = 20);
    // Connect to a server asynchronously
    void ConnectAsync(const tcp::endpoint& peer, bool* threadEnd = NULL, size_t* connectionId = NULL, uint32_t secondsToWait =20);
    // Disconnect to a server
    void Disconnect(size_t connectionId);
    // Check if connected
    bool IsConnected(size_t connectionId);

    // Start handling all requests that the client gets
    void HandleRequests();
    // Start handling all requests asynchronously
    void HandleRequestsAsync();
    
    // Set the event handler to handle incoming chat messages
    void SetMessageEventHandler(std::function<void(boost::shared_ptr<MessageEventData>)> handler) { m_messageHandler = handler; }
    // Send chat message to a server
    void SendMessage(size_t receiverId, const std::string& message, uint32_t groupId = 0);

    // Send a join chat request; on successful returns true
    bool JoinChat(uint32_t connectionId, uint32_t groupId = 0);
    // Send a join video chat request; on successful return true
    bool JoinVideoChat(uint32_t connectionId, uint32_t groupId = 0);
    // Set username to use while sending messages
    void SetName(const std::string &name) { m_name = name; }
    // Get username used while sending messages
    const std::string& GetName() { return m_name; }
    // Set connectionId of server
    void SetServer(size_t connectionId);
    // Get connectionId of server
    size_t GetServer() const { return m_serverId; }

    // Start receiving Audio-Video asynchronously
    void StartReceivingAV(VideoPlayback* videoPlaybackHandler, VideoCapture* videoCapture);
    // Stop receviing Audio-Video data and clear all received units
    void StopReceivingAV();
    // RTP Streamer
    RtpStreamer& GetRtpStreamer() { return m_rtpStreamer; }

    // Get remote udp endpoint of a connection
    const udp::endpoint& GetUdpEndpoint(size_t connectionId) const { return m_connections[connectionId].udpEndpoint; }
    UdpHandler& GetUdpHandler1() { return m_udpHandler1; }
    UdpHandler& GetUdpHandler2() { return m_udpHandler2; }

private:
    boost::asio::io_service m_io;
    // Event handlers
    std::function<void(boost::shared_ptr<MessageEventData>)> m_messageHandler;
    std::function<bool(size_t)> m_joinChatHandler;

    // Connection representing a tcp-connection with a server
    struct Connection
    {
        Connection(boost::asio::io_service &io)
        : tcpHandler(io)
        {}
        TcpHandler tcpHandler;
        udp::endpoint udpEndpoint;
        bool connected;
    };
    // Single udp handler can be used for all connections
    // We need two udp handlers (even and odd pair) for rtp and rtcp
    UdpHandler m_udpHandler1;
    UdpHandler m_udpHandler2;

    // Streamer to send/receive av data
    RtpStreamer m_rtpStreamer;
    VideoPlayback* m_videoPlayback;
    VideoCapture* m_videoCapture;

    // List of the connections
    std::vector<Connection> m_connections;
    // Mutex to lock the use of common variables (m_connections/m_request/...) during multithreading
    boost::mutex m_mutex;
    // Connection-Id of server
    size_t m_serverId;
    // Id of this client as recorded by the server
    size_t m_clientId;

    TcpRequest m_request;
    std::string m_name;

    void ConnectTimer(TcpHandler &handler, uint32_t seconds);
};
