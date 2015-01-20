#pragma once
#include <common/RtpPacket.h>

// A RTP streamer class that can be used to stream in/out
// data as fragmented RTP packets
class RtpStreamer
{
public:
    void Initialize(UdpHandler* udpHandler) { m_udpHandler = udpHandler; }
    
    // Send the data as fragmented RTP packets
    void Send(RtpPacket& rtp, uint8_t* data, size_t len);
    // Receive fragmented RTP packets into single memory block
    // An allocater needs to be passed to allocate the memory block
    // WARNING: DONOT USE
    size_t Receive(RtpPacket& rtp, uint8_t** data, void* (*allocator)(size_t));


    // Start receiving Rtp packets and keeping the order of their sequence number
    //  merge the fragmented ones into single unit
    void StartReceiving();
    // Stop the receiving process
    void StopReceiving();

    // Get a received packet of data; the fragmented rtp packets are merged into one and returned
    size_t GetPacket(uint32_t source, uint8_t** data, void* (*allocator)(size_t));
    
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
    
    // Map each source with a list of RtpUnit's that it sends
    typedef std::vector<RtpUnit> RtpUnitList;
    std::map<uint32_t, RtpUnitList> m_rtpUnits;
    
    size_t m_startId;

    boost::mutex m_mutex;
    bool m_receiving;
};
