#pragma once



#include "client/MediaStream/MediaStream.h"




class VideoStream :public MediaStream
{
public:
    AVCodecID m_codecID;
    AVCodec *m_codec;
    AVCodecContext *m_codecContext;
    std::vector<AVPacket> m_encodedFrames;
    std::vector<AVPacket> m_delayedFrames;
    VideoStream()
    {
        avcodec_register_all();
        m_codecID = AV_CODEC_ID_H264;
        m_codec = avcodec_find_encoder(m_codecID);
        if (!m_codec) throw CodecMissingException();
        m_codecContext = avcodec_alloc_context3(m_codec);
        if (!m_codecContext) throw Exception("can't allocate video codec context");
        m_codecContext->bit_rate = 100000;
        m_codecContext->width = 1280;
        m_codecContext->height = 768;
        AVRational avrat = { 1, 25 };   // for 25 fps
        m_codecContext->time_base = avrat;
        /*emit one intra frame every ten frames
        check frame pict_type before passing frame
        to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
        then gop_size is ignored and the output of encoder
        will always be I frame irrespective to gop_size*/
        m_codecContext->gop_size = 10;
        m_codecContext->max_b_frames = 1;
        m_codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

        if (m_codecID == AV_CODEC_ID_H264)
            av_opt_set(m_codecContext->priv_data, "preset", "slow", 0);

        if (avcodec_open2(m_codecContext, m_codec, NULL) < 0) 
            throw Exception("failed to open the codec");

    }
    std::vector<AVPacket> GrabDelayedPackets()
    {
        // there shouldn't be any delayed frames in our case

        /*AVPacket pkt;
        int packetFilled = 1;
        for (; packetFilled; ) {
            int ret = avcodec_encode_video2(m_codecContext, &pkt, NULL, &packetFilled);
            if (ret < 0) throw Exception("failed to encode delayed frames");

            if (packetFilled) {
                
                av_free_packet(&pkt);
            }
        }*/
    }
    unsigned int AllocateNewPacket()
    {
        m_encodedFrames.resize(m_encodedFrames.size() + 1);
        return m_encodedFrames.size() - 1;
    }
    void AddFrame(AVFrame *frame)
    {
        unsigned int packetIndex = AllocateNewPacket();
        AVPacket* pkt = &m_encodedFrames[packetIndex];
        av_init_packet(pkt);
        pkt->data = NULL;    // packet data will be allocated by the encoder
        pkt->size = 0;

        // The output packet doesn't necessarily contain encoded data for most
        // recent frame, as encoder can delay and reorder input frames internally
        // as required
        // packetFilled checks if the packet is delayed or not
        int packetFilled = 0;
        if (avcodec_encode_video2(m_codecContext, pkt, frame, &packetFilled) < 0)
        {
            throw Exception("failed to encode the frame");
        }

        if (packetFilled) 
        {
            
        }
    }
    //std::vector
    void Test()
    {

        SwsContext * ctx = sws_getContext(m_codecContext->width, m_codecContext->height,
            AV_PIX_FMT_RGB24, m_codecContext->width, m_codecContext->height,
            AV_PIX_FMT_YUV420P, 0, 0, 0, 0);
        uint8_t* rgb24Data = new uint8_t[3 * m_codecContext->width*m_codecContext->height];
        uint8_t * inData[1] = { rgb24Data };                // RGB24 have one plane
        int inLinesize[1] = { 3 * m_codecContext->width };  // RGB stride



        AVFrame *frame = avcodec_alloc_frame();
        frame->width = m_codecContext->width;
        frame->height = m_codecContext->height;
        frame->format = m_codecContext->pix_fmt;
        // the image can be allocated by any means and av_image_alloc() is
        // just the most convenient way if av_malloc() is to be used 
        if (av_image_alloc(frame->data, frame->linesize, m_codecContext->width,
            m_codecContext->height, m_codecContext->pix_fmt, 32) < 0)
        {
            throw Exception("failed to allocate raw picture buffer");
        }
        for (int i = 0; i < 25 * 10; i++)
        {
            for (int y = 0; y < frame->height; y++)
            {
                for (int x = 0; x < frame->width; x++)
                {
                    rgb24Data[3 * (y*frame->width + x) + 0] = i % 255;
                    rgb24Data[3 * (y*frame->width + x) + 1] = i % 255;
                    rgb24Data[3 * (y*frame->width + x) + 2] = i % 255;
                }
                
            }
            sws_scale(ctx, inData, inLinesize, 0, frame->height, frame->data, frame->linesize);
            frame->pts = i;
            this->AddFrame(frame);
        }
        FILE* fp = fopen("test.avi", "wb");
        for (int i = 0; i < m_encodedFrames.size(); i++)
        {
            fwrite(m_encodedFrames[i].data, 1, m_encodedFrames[i].size, fp);
        }
        uint8_t endcode[] = { 0, 0, 1, 0xb7 };
        fwrite(endcode, 1, sizeof(endcode), fp);
        fclose(fp);
    }
protected:
    
};
