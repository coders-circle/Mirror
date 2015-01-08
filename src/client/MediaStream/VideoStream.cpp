#include "common/common.h"
#include "client/MediaStream/VideoStream.h"

void VideoStream::Encode()
{
    FILE* fp = fopen("test.avi", "wb");
    for (unsigned int i = 0; i < m_encodedPackets.size(); i++)
    {
        fwrite(m_encodedPackets[i]->data, 1, m_encodedPackets[i]->size, fp);
    }
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fwrite(endcode, 1, sizeof(endcode), fp);
    fclose(fp);
}


void VideoStream::Test()
{
    this->InitializeEncoder(640, 480, 15);
    int w = m_encoderContext->width;
    int h = m_encoderContext->height;
    uint8_t *rgb24Data = new uint8_t[w*h * 3];
    for (int i = 0; i < 15 * 17; i++)
    {
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                rgb24Data[3 * (y*w + x) + 0] = i % 255;
                rgb24Data[3 * (y*w + x) + 1] = 0;
                rgb24Data[3 * (y*w + x) + 2] = 0;
            }
        }
        this->AddFrame(rgb24Data, i);
    }
    /*FILE* fp = fopen("test.flv", "wb");
    for (unsigned int i = 0; i < m_encodedPackets.size(); i++)
    {
        fwrite(m_encodedPackets[i]->data, 1, m_encodedPackets[i]->size, fp);
    }
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fwrite(endcode, 1, sizeof(endcode), fp);
    fclose(fp);*/

    this->InitializeDecoder();
    for (unsigned int i = 0; i < m_encodedPackets.size(); i++)
    {
        this->AddPacket(m_encodedPackets[i]);
        av_free_packet(m_encodedPackets[i]);
    }
}


VideoStream::VideoStream() 
{
    m_codecID = AV_CODEC_ID_H264;
    m_RGB24ToYUP420PConverterContext = 0;
    m_YUV420PToRGB24ConverterContext = 0;
    m_fw = 0;
    m_fh = 0;
    m_fps = 0;
}

VideoStream::~VideoStream()
{
    if (m_RGB24ToYUP420PConverterContext)
    {
        sws_freeContext(m_RGB24ToYUP420PConverterContext);
    }
    if (m_YUV420PToRGB24ConverterContext)
    {
        sws_freeContext(m_YUV420PToRGB24ConverterContext);
    }
}

void VideoStream::InitializeDecoder()
{
    this->SetupDecoder();
    this->OpenCodec(m_decoderContext, m_decoder);
}

void VideoStream::AddPacket(uint8_t *packetData, int packetSize)
{
    AVPacket* pkt = new AVPacket();
    av_init_packet(pkt);
    pkt->data = packetData;
    pkt->size = packetSize;
    this->AddPacket(pkt);
}

void VideoStream::AddPacket(AVPacket* pkt)
{
    AVFrame* frame = av_frame_alloc();
    int framePresent = 0;

    if (avcodec_decode_video2(m_decoderContext, frame, &framePresent, pkt) < 0)
    {
        throw FailedToDecode();
    }

    if (framePresent)
    {
        if (frame->width != 0)
        {
            while (!m_decodedFrameLock.try_lock())
                std::cout << "Addpacket waiting" << std::endl;
            unsigned int frameIndex = this->AllocateNewDecodedFrame();
            m_decodedFrames[frameIndex] = av_frame_clone(frame);            
            if (!m_decodedFrames[frameIndex])
            {
                m_decodedFrames[frameIndex] = av_frame_alloc();
            }
            av_frame_copy(m_decodedFrames[frameIndex], frame);
            m_decodedFrameLock.unlock();
            if (m_rawData.size() == 0)
            {
                // this is the first time we get the filled frame
                // so allocate enough memory for raw data and
                // initialize scaler context for color fromat conversion
                this->AllocateRawData(frame->height*frame->width * 3);
                if (m_YUV420PToRGB24ConverterContext)
                {
                    //should not happen
                    sws_freeContext(m_YUV420PToRGB24ConverterContext);
                }
                
                m_fw = frame->width;
                m_fh = frame->height;
                m_fps = m_decoderContext->time_base.den/(m_decoderContext->time_base.num*m_decoderContext->ticks_per_frame);
                m_YUV420PToRGB24ConverterContext = sws_getContext(m_fw, m_fh, AV_PIX_FMT_YUV420P, 
                    m_fw, m_fh, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
            }
        }
    }
    else
    {
        if (avcodec_decode_video2(m_decoderContext, NULL, &framePresent, pkt) < 0)
        {
            throw FailedToDecode();
        }
        if (framePresent)
        {
            std::cout << "OMG delayed packet" << std::endl;
        }
    }
}


unsigned char* VideoStream::GetRawRGBData(unsigned int decodedFrameIndex)
{
    if (decodedFrameIndex >= m_decodedFrames.size())
    {
        throw Exception("invalid frame index");
    }
    int w = m_decodedFrames[decodedFrameIndex]->width;
    int h = m_decodedFrames[decodedFrameIndex]->height;
    if (w != 0)
    {
        AVFrame* yuvFrame = m_decodedFrames[decodedFrameIndex];
        AVPicture *rgbFrame = new AVPicture;
        avpicture_alloc(rgbFrame, AV_PIX_FMT_RGB24, w, h);
        sws_scale(m_YUV420PToRGB24ConverterContext, yuvFrame->data, yuvFrame->linesize, 0, h, rgbFrame->data, rgbFrame->linesize);
        memcpy(m_rawData.data(), rgbFrame->data[0], w*h * 3);
        avpicture_free(rgbFrame);
    }
    return m_rawData.data();
}

void VideoStream::InitializeEncoder(int w, int h, int fps, int bitrate)
{
    this->SetupEncoder();
    AVRational avrat = { 1, fps };

    m_encoderContext->bit_rate = bitrate;
    m_encoderContext->width = w;
    m_encoderContext->height = h;
    m_encoderContext->time_base = avrat;
    m_encoderContext->gop_size = 30;
    m_encoderContext->max_b_frames = 0;
    m_encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (m_codecID == AV_CODEC_ID_H264)
    {
       // av_opt_set(m_encoderContext->priv_data, "preset", "slow", 0);
        av_opt_set(m_encoderContext->priv_data, "tune", "zerolatency", 0);
        //av_dict_set
    }
    this->OpenCodec(m_encoderContext, m_encoder);

    m_RGB24ToYUP420PConverterContext = sws_getContext(m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_RGB24, m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);

    m_fw = w;
    m_fh = h;
    m_fps = fps;
}


void VideoStream::AddFrame(uint8_t* rgb24Data, uint64_t pts)
{
    AVFrame *frame = av_frame_alloc();

    frame->width = m_encoderContext->width;
    frame->height = m_encoderContext->height;
    frame->format = m_encoderContext->pix_fmt;
    frame->pts = pts;

    if (av_image_alloc(frame->data, frame->linesize, m_encoderContext->width,
        m_encoderContext->height, m_encoderContext->pix_fmt, 32) < 0)
    {
        throw Exception("failed to allocate raw picture buffer");
    }

    // Convert RGB data to YUV before adding the frame for encoding
    uint8_t * inData[1] = { rgb24Data };                    // RGB24 have only one plane
    int inLinesize[1] = { 3 * m_encoderContext->width };    // RGB stride
    sws_scale(m_RGB24ToYUP420PConverterContext, inData, inLinesize, 0, frame->height, frame->data, frame->linesize);

    // Add frame for encoding
    this->AddFrame(frame);
    av_frame_free(&frame);
}


void VideoStream::AddFrame(AVFrame *frame)
{
    //boost::lock_guard<boost::mutex> guard(m_frameLock);
    while (!m_encodedPacketLock.try_lock())
        std::cout << "AddFrame waiting..." << std::endl;
    unsigned int packetIndex = this->AllocateNewEndodedPacket();
    m_encodedPackets[packetIndex] = new AVPacket;
    av_init_packet(m_encodedPackets[packetIndex]);
    AVPacket* pkt = m_encodedPackets[packetIndex];
    pkt->data = NULL;   // packet data will be allocated by the encoder
    pkt->size = 0;

    // The output packet doesn't necessarily contain encoded data for most
    // recent frame, as encoder can delay and reorder input frames internally
    // as required.
    //
    // But in our case packets should never be delayed.
    //
    // 'packetFilled' checks if the packet is delayed or not
    int packetFilled = 0;
    if (avcodec_encode_video2(m_encoderContext, pkt, frame, &packetFilled) < 0)
    {
        m_encodedPacketLock.unlock();
        throw FailedToEncode();
    }
    m_encodedPacketLock.unlock();
}

// Test methods to send/receive the video over RTP

#include <common/RtpPacket.h>
#include <client/MediaStream/RtpStreamer.h>
#include <client/Client.h>
#define min(x,y) ((x)<(y)?(x):(y))

void VideoStream::SendRtp(Client &client, size_t connectionId)
{
    //boost::lock_guard<boost::mutex> guard(m_frameLock);
    // We will use a RTP streamer to stream out the encoded packets
    //  over as fragmented RTP packets
    size_t pktsSz;
    RtpPacket rtp;
    RtpStreamer rtpstr;

    // Initialize the sending parameters for the RTP packets
    rtp.Initialize(&client.GetUdpHandler1(), client.GetUdpEndpoint(connectionId));
    rtp.SetPayloadType(123);

    // Use RTP streamer to send each packet that has been encoded
    while (!m_encodedPacketLock.try_lock())
        std::cout << "SendRtp waiting..." << std::endl;

    if (m_encodedPackets.size() > 0 ){
        if (m_encodedPackets[0]->size != 0)
            rtpstr.Send(rtp, m_encodedPackets[0]->data, m_encodedPackets[0]->size);
        std::cout << "Available Packets: " << m_encodedPackets.size() << std::endl;
        EraseEncodedPacketFromHead();
    }
    //pktsSz = m_encodedPackets.size();
    //for (size_t i = 0; i < pktsSz; ++i)
    //{
    //    AVPacket* pkt = m_encodedPackets[i];
    //    rtpstr.Send(rtp, pkt->data, pkt->size);
    //}
    //// Erase the sent packets
    //EraseEncodedPacketFromHead(pktsSz);
    m_encodedPacketLock.unlock();
}

void VideoStream::ReceiveRtp(Client &client)
{
    if (client.GetUdpHandler1().GetSocket()->available() == 0) 
        return;
    //boost::lock_guard<boost::mutex> guard(m_frameLock);

    // Again, a RTP streamer is used to stream in the fragmented packets
    //  into one single encoded video packet
    RtpPacket rtp;
    udp::endpoint ept;
    rtp.Initialize(&client.GetUdpHandler1(), ept);
    // The streamer will also allocate the memory for the encoded packet
    //  for which we pass an allocator (av_malloc)
    uint8_t* pdata;
    RtpStreamer rstr;
    size_t len = rstr.Receive(rtp, &pdata, av_malloc);
    if (len == 0) return;

    // Add the packet to our record; this can now be decoded and played
    //AVPacket* pkt = m_encodedPackets[AllocateNewEndodedPacket()] = new AVPacket;
    //av_init_packet(pkt);
    //pkt->data = pdata;
    //pkt->size = len;
    
    std::cout << "Received packet: " << len << std::endl;
    this->AddPacket(pdata, len);
}
