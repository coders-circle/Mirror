#pragma once

class RtpStreamer
{
public:
    RtpStreamer();
    ~RtpStreamer();

    void Initialize(boost::shared_ptr<boost::asio::ip::udp::socket> socket, const boost::asio::ip::address &clientAddress);
    void CleanUp();

private:
    boost::shared_ptr<boost::asio::ip::udp::socket> m_socket;
    boost::asio::ip::address m_clientAddress;
};