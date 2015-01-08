#pragma once
#include <common/RtpPacket.h>

// A RTP streamer class that can be used to stream in/out
// data as fragmented RTP packets
class RtpStreamer
{
public:
    // Send the data as fragmented RTP packets
    void Send(RtpPacket& rtp, uint8_t* data, size_t len);
    // Receive fragmented RTP packets into single memory block
    // An allocater needs to be passed to allocate the memory block
    size_t Receive(RtpPacket& rtp, uint8_t** data, void* (*allocator)(size_t));

private:

};
