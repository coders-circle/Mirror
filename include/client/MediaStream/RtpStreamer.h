#pragma once
#include <common/RtpPacket.h>

class RtpStreamer
{
public:
    void Send(RtpPacket& rtp, uint8_t* data, size_t len);
    size_t Receive(RtpPacket& rtp, uint8_t** data, void* (*allocator)(size_t));

private:

};
