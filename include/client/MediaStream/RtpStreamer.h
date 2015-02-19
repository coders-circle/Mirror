#pragma once
#include <common/RtpPacket.h>

// A RTP streamer class that can be used to stream in/out
// data as fragmented RTP packets
class RtpStreamer
{
public:
    void Initialize(UdpHandler* udpHandler) { m_udpHandler = udpHandler; }
    
    // Send the data as fragmented RTP packets
    void Send(RtpPacket& rtp, const uint8_t* data, size_t len);
    
    // Start receiving Rtp packets and keeping the order of their sequence number
    //  merge the fragmented ones into single unit
    void StartReceiving();
    // Stop the receiving process
    void StopReceiving();

    // Get a received packet of data; the fragmented rtp packets are merged into one and returned
    size_t GetPacket(uint32_t source, uint8_t mediaType, uint8_t** data, void* (*allocator)(size_t));
    // Get a list of available sources
    std::vector<uint32_t> GetSources(uint8_t mediaType);
    
    // The udp handler this streamer used to send/receive rtp packets
    UdpHandler* GetUdpHandler() { return m_udpHandler; }

private:
    // Holder for data in each RTP packet
    struct RtpUnit
    {
        std::vector<uint8_t> data;
        bool operator<(const RtpUnit& other) const
        {
            return sn < other.sn;
        }
        
        uint16_t sn;        // sequence number to order the incoming packets
        bool marker;        // marker marks end of an encoded frame
    };

    UdpHandler* m_udpHandler;

    struct RtpUnitList
    {
        RtpUnitList() : begin(list.begin()) {}
        std::list<RtpUnit> list;
        std::list<RtpUnit>::iterator begin;
    };
    
    // Map each source and media type with a list of RtpUnit's that it sends
    std::map<std::pair<uint32_t, uint8_t>, RtpUnitList> m_rtpUnits;
    boost::mutex m_mutex;
    bool m_receiving;
};
