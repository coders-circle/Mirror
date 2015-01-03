#pragma once



#include "client/MediaStream/MediaStream.h"

#ifndef _WIN32
#define av_frame_alloc avcodec_alloc_frame
#endif




class VideoStream :public MediaStream
{
public:
    AVCodecID m_codecID;
    AVCodec *m_codec;
    AVCodecContext *m_codecContext;
    std::vector<AVPacket> m_encodedFrames;
    std::vector<AVPacket> m_delayedFrames;
    SwsContext * m_ctx;
    void Initialize(int w, int h, int fps = 15, int bitrate = 100000)
    {
        m_codecID = AV_CODEC_ID_H264;
        m_codec = avcodec_find_encoder(m_codecID);
        if (!m_codec) throw CodecMissingException();
        m_codecContext = avcodec_alloc_context3(m_codec);
        if (!m_codecContext) throw Exception("can't allocate video codec context");
        m_codecContext->bit_rate = bitrate;
        m_codecContext->width = w;
        m_codecContext->height = h;
        AVRational avrat = { 1, fps };   // for 25 fps
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

        m_ctx = sws_getContext(m_codecContext->width, m_codecContext->height,
            AV_PIX_FMT_RGB24, m_codecContext->width, m_codecContext->height,
            AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    }
    VideoStream()
    {
        

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

    // pts = presentation time stamp
    void AddFrame(uint8_t* rgb24Data, int pts)
    {
        uint8_t * inData[1] = { rgb24Data };                // RGB24 have one plane
        int inLinesize[1] = { 3 * m_codecContext->width };  // RGB stride

        AVFrame *frame = av_frame_alloc();
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
        sws_scale(m_ctx, inData, inLinesize, 0, frame->height, frame->data, frame->linesize);
        frame->pts = pts;
        this->AddFrame(frame);


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
            std::cout << "Filled" << std::endl;
        }
    }
    //std::vector
    void Test()
    {
        int w = m_codecContext->width;
        int h = m_codecContext->height;
        uint8_t *rgb24Data = new uint8_t[w*h * 3];
        for (int i = 0; i < 25 * 10; i++)
        {
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++)
                {
                    rgb24Data[3 * (y*w + x) + 0] = i % 255;
                    rgb24Data[3 * (y*w + x) + 1] = i % 255;
                    rgb24Data[3 * (y*w + x) + 2] = i % 255;
                }
                
            }
            this->AddFrame(rgb24Data, i);
        }
        FILE* fp = fopen("test.flv", "wb");
        for (int i = 0; i < m_encodedFrames.size(); i++)
        {
            fwrite(m_encodedFrames[i].data, 1, m_encodedFrames[i].size, fp);
        }
        uint8_t endcode[] = { 0, 0, 1, 0xb7 };
        fwrite(endcode, 1, sizeof(endcode), fp);
        fclose(fp);
    }

    // Test Encode to File
    void Encode()
    {
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
