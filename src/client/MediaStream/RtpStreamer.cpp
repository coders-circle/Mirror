#include <common/common.h>
#include <client/MediaStream/RtpStreamer.h>

// Send the data as fragmented RTP packets
void RtpStreamer::Send(RtpPacket& rtp, uint8_t* data, size_t len)
{
    // Each RTP packet as max-length 1024
    size_t slen = 1024; // Max-Size of each rtp packet
    size_t plen = 0;    // Length of processed data (data that has been sent)

    // We set the marker bit on rtp header as 0 till the last frame
    rtp.SetMarker(false);
    while (plen < len)
    {
        if (slen >= len-plen) 
        {
            // On last frame, we set the marker bit to 1
            slen = len-plen;
            rtp.SetMarker(true);
        }
        // Send the fragmented packet
        rtp.Send(data + plen, slen);
        plen += slen;
    }
}

// Receive fragmented RTP packets into single memory block
size_t RtpStreamer::Receive(RtpPacket& rtp, uint8_t** data, void* (*allocator)(size_t))
{
    // A vector of fragments where each fragment is a vector of bytes
    std::vector<std::vector<uint8_t>> datas;
    size_t tlen = 0;    // The total length of received data

    // We receive all RTP packets for which marker bit on rtp header is unset
    // and only break the process when marker bit is set on the received packet
    do
    {
        datas.push_back(std::vector<uint8_t>(1024));
        size_t sz = rtp.Receive(&datas[datas.size()-1][0], 1024);
        datas[datas.size()-1].resize(sz);
        tlen += sz;
    } while (!rtp.IsMarkerSet());

    // Allocate memory for the total received data and send back the pointer
    uint8_t* newdata = *data = (uint8_t*)allocator(tlen);
    // In the allocated memory, copy all data from each fragment
    for (size_t i = 0; i < datas.size(); ++i)
    {
        memcpy(newdata, &datas[i][0], datas[i].size());
        newdata += datas[i].size();
    }
    return tlen;
}
