#include <common/common.h>
#include <client/MediaStream/RtpStreamer.h>

// Send the data as fragmented RTP packets
void RtpStreamer::Send(RtpPacket& rtp, uint8_t* data, size_t len)
{
    boost::lock_guard<boost::mutex> guard(m_mutex);

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

// Get a received frame of data; rtp packets are merged till the packet that contains the marker bit
size_t RtpStreamer::GetPacket(uint32_t source, uint8_t** data, void* (*allocator)(size_t))
{
    boost::lock_guard<boost::mutex> guard(m_mutex);

    if (m_rtpUnits.find(source) == m_rtpUnits.end())
        return 0;
    
    RtpUnitList& rtunits = m_rtpUnits[source];
    if (rtunits.list.size() == 0) 
        return 0;

    // Find the length of the frame and the last rtp packet that contains the marker bit
    // The total length is all length of all packets till the last rtp packet
    size_t tlen = 0;
    if (rtunits.begin == rtunits.list.end())
        rtunits.begin = rtunits.list.begin();
    auto it = rtunits.begin;
    while (true)
    {
        RtpUnit& unit = *it;
        tlen += unit.data.size();
        if (unit.marker)
            break;
        it++;
        if (it == rtunits.list.end())
            it = rtunits.list.begin();
        if (it == rtunits.begin)
            return 0;
    }
    
    // Allocate memory for the total encoded frame and send back the pointer
    uint8_t* newdata = *data = (uint8_t*)allocator(tlen);
    // In the allocated memory, copy all data from each fragment and remove the fragment from list
    auto jt = rtunits.begin;
    while (true)
    {
        if (jt == rtunits.list.end())
            jt = rtunits.list.begin();
        memcpy(newdata, &jt->data[0], jt->data.size());
        newdata += jt->data.size();
        auto toerase = jt;
        if (jt == it) 
            break;
        jt++;

        rtunits.list.erase(toerase);
    }
    rtunits.begin = ++it;
    
    return tlen;
}

std::vector<uint32_t> RtpStreamer::GetSources()
{
    std::vector<uint32_t> sources;
    for (auto it = m_rtpUnits.begin(); it != m_rtpUnits.end(); ++it)
        if (it->second.list.size() > 0)
            sources.push_back(it->first);

    return sources;
}

// Start receiving Rtp packets
void RtpStreamer::StartReceiving()
{
    m_receiving = true;
    RtpPacket rtp;
    rtp.Initialize(m_udpHandler, udp::endpoint());
    // Receive the packets till StopReceiving is called
    while (m_receiving)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
        // sleep(...)
        if (m_udpHandler->GetSocket()->available() > 0)
        {
            boost::lock_guard<boost::mutex> guard(m_mutex);
            // Max size of each rtp packet is 1024
            RtpUnit unit;
            unit.data.resize(1024);
            size_t len = rtp.Receive(&unit.data[0], 1024);
            if (len == 0) 
                continue;
            
            // Add each rtp packet to the list and sort it
            unit.data.resize(len);
            unit.marker = rtp.IsMarkerSet();
            unit.sn = rtp.GetSequenceNumber();
            m_rtpUnits[rtp.GetSourceId()].list.push_back(std::move(unit));
            m_rtpUnits[rtp.GetSourceId()].list.sort();
            //std::sort(m_rtpUnits[rtp.GetSourceId()].list.begin(), m_rtpUnits[rtp.GetSourceId()].list.end());
        }
    }
}

// Stop receiving Rtp packets; Also clear all list of RtpUnits for all sources
void RtpStreamer::StopReceiving()
{
    boost::lock_guard<boost::mutex> guard(m_mutex);
    m_receiving = false;
    m_rtpUnits.clear();
}
