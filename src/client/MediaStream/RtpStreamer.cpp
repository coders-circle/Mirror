#include <common/common.h>
#include <client/MediaStream/RtpStreamer.h>

void RtpStreamer::Send(RtpPacket& rtp, uint8_t* data, size_t len)
{
    size_t slen = 1024;
    size_t plen = 0;
    rtp.SetMarker(false);
    while (plen < len)
    {
        if (slen >= len-plen) 
        {
            slen = len-plen;
            rtp.SetMarker();
        }
        rtp.Send(data + plen, slen);
        plen += slen;
    }
}

size_t RtpStreamer::Receive(RtpPacket& rtp, uint8_t** data, void* (*allocator)(size_t))
{
    std::vector<std::vector<uint8_t>> datas;
    size_t tlen;
    do
    {
        datas.push_back(std::vector<uint8_t>(1024));
        size_t sz = rtp.Receive(&datas[datas.size()-1][0], 1024);
        datas[datas.size()-1].resize(sz);
        tlen += sz;
    } while (!rtp.IsMarkerSet());

    uint8_t* newdata =  *data = (uint8_t*)allocator(tlen);
    for (size_t i = 0; i < datas.size(); ++i)
    {
        memcpy(newdata, &datas[i][0], datas[i].size());
        newdata += datas[i].size();
    }
    return tlen;
}
