#pragma once

#include "client/MediaStream/MediaStream.h"

#ifndef _WIN32
#define av_frame_alloc avcodec_alloc_frame
#endif

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096




class VideoStream :public MediaStream
{
public:
    AVCodecID m_codecID;
    AVCodec *m_encoder;
    AVCodec *m_decoder;
    AVCodecContext *m_encoderContext;
    AVCodecContext *m_decoderContext;

    // contains encoded frames
    std::vector<AVPacket> m_encodedFrames;

    // not used in our case, for the containers like mp4
    // which can generate delayed frames
    std::vector<AVPacket> m_delayedFrames;

    // contains decoded raw frames
    std::vector<AVFrame*> m_rawFrames;
    SwsContext * m_ctx;
    uint8_t* m_rawRGBFrameData;

    AVCodecParserContext* m_parser;

    void InitializeDecoder()
    {
        m_decoder = avcodec_find_decoder(m_codecID);

        if (!m_decoder) throw CodecMissingException();
        m_decoderContext = avcodec_alloc_context3(m_decoder);
        if (!m_decoderContext) throw Exception("Could not allocate video codec context");

        if (avcodec_open2(m_decoderContext, m_decoder, NULL) < 0) 
            throw Exception("Could not open codec");

        //m_parser = av_parser_init(AV_CODEC_ID_H264);
        m_rawRGBFrameData = 0;

    }
    void AddFrame(AVPacket* pkt)
    {
        unsigned int packetIndex = this->AllocateNewRawPacket();
        //std::cout << packetIndex << std::endl;
        AVFrame* frame = av_frame_alloc();
        //m_rawFrames[packetIndex] = av_frame_alloc();
        //uint8_t* data = NULL;
        //int size = 0;
        //av_parser_parse2(m_parser, m_decoderContext, &data, &size, pkt->data, pkt->size, 0, 0, AV_NOPTS_VALUE);
        
        int framePresent = 0;

        if (avcodec_decode_video2(m_decoderContext, frame, &framePresent, pkt) < 0)
        {
            throw Exception("error decoding the packet");
        }
        m_rawFrames[packetIndex] = av_frame_clone(frame);
        if (!m_rawFrames[packetIndex])
            m_rawFrames[packetIndex] = av_frame_alloc();
        //av_frame_clo
        av_frame_copy(m_rawFrames[packetIndex], frame);
        if (framePresent)
        {
            //std::cout << "Frame present";
            if (!m_rawRGBFrameData && frame->width != 0)
            {
                m_rawRGBFrameData = new uint8_t[frame->height*frame->width * 3];
                std::cout << "Done!";
            }
        }
    }
    unsigned char* GetRawRGBData(unsigned int rawFrameIndex)
    {
        if (rawFrameIndex >= m_rawFrames.size())
        {
            throw Exception("invalid frame index");
        }
        int w = m_rawFrames[rawFrameIndex]->width;
        int h = m_rawFrames[rawFrameIndex]->height;
        if (w != 0)
        {
            static SwsContext* scalerContext = sws_getContext(w, h, AV_PIX_FMT_YUV420P, w, h,
                AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
            AVFrame* yuvFrame = m_rawFrames[rawFrameIndex];   // RGB24 have one plane
            //AVFrame *rgbFrame = av_frame_alloc();
            int outLinesize[] = { 0 };  // RGB stride
            uint8_t* outData[] = { m_rawRGBFrameData };
            //rgbFrame->data[0] = m_rawRGBFrameData;
            AVPicture *rgbFrame = new AVPicture;
            avpicture_alloc(rgbFrame, AV_PIX_FMT_RGB24, w, h);
            sws_scale(scalerContext, yuvFrame->data, yuvFrame->linesize, 0, h, rgbFrame->data, rgbFrame->linesize);
            memcpy(m_rawRGBFrameData, rgbFrame->data[0], w*h * 3);
            avpicture_free(rgbFrame);
        }
        return m_rawRGBFrameData;
    }
    void InitializeEncoder(int w, int h, int fps = 15, int bitrate = 100000)
    {
        m_encoder = avcodec_find_encoder(m_codecID);
        if (!m_encoder) throw CodecMissingException();
        m_encoderContext = avcodec_alloc_context3(m_encoder);
        if (!m_encoderContext) throw Exception("can't allocate video codec context");
        m_encoderContext->bit_rate = bitrate;
        m_encoderContext->width = w;
        m_encoderContext->height = h;
        AVRational avrat = { 1, fps }; 
        m_encoderContext->time_base = avrat;
        m_encoderContext->gop_size = 10;
        m_encoderContext->max_b_frames = 1;
        m_encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
        
        if (m_codecID == AV_CODEC_ID_H264)
            av_opt_set(m_encoderContext->priv_data, "preset", "slow", 0);

        if (avcodec_open2(m_encoderContext, m_encoder, NULL) < 0)
            throw Exception("failed to open the codec");

        m_ctx = sws_getContext(m_encoderContext->width, m_encoderContext->height,
            AV_PIX_FMT_RGB24, m_encoderContext->width, m_encoderContext->height,
            AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    }
    VideoStream()
    {
        m_codecID = AV_CODEC_ID_H264;
    }
    void GrabDelayedPackets()
    {
        // there shouldn't be any delayed frames in our case
        // following is the code just in case
        /*
        AVPacket pkt;
        int packetFilled = 1;
        for (; packetFilled; ) 
        {
            if(avcodec_encode_video2(m_encoderContext, &pkt, NULL, &packetFilled) < 0)
                throw Exception("failed to encode delayed frames");

            if (packetFilled) {
                // store the packet
            }
        }
        */
            }
    unsigned int AllocateNewRawPacket()
    {
        m_rawFrames.resize(m_rawFrames.size() + 1);
        return (m_rawFrames.size() - 1);
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
        int inLinesize[1] = { 3 * m_encoderContext->width };  // RGB stride

        AVFrame *frame = av_frame_alloc();
        frame->width = m_encoderContext->width;
        frame->height = m_encoderContext->height;
        frame->format = m_encoderContext->pix_fmt;
        if (av_image_alloc(frame->data, frame->linesize, m_encoderContext->width,
            m_encoderContext->height, m_encoderContext->pix_fmt, 32) < 0)
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
        if (avcodec_encode_video2(m_encoderContext, pkt, frame, &packetFilled) < 0)
        {
            throw Exception("failed to encode the frame");
        }

        if (packetFilled) 
        {
            // this should always occur for our case
        }
    }
    void Test()
    {
        this->InitializeEncoder(640, 480);
        int w = m_encoderContext->width;
        int h = m_encoderContext->height;
        uint8_t *rgb24Data = new uint8_t[w*h * 3];
        for (int i = 0; i < 25 * 10; i++)
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
        FILE* fp = fopen("test.flv", "wb");
        for (unsigned int i = 0; i < m_encodedFrames.size(); i++)
        {
            fwrite(m_encodedFrames[i].data, 1, m_encodedFrames[i].size, fp);
        }
        uint8_t endcode[] = { 0, 0, 1, 0xb7 };
        fwrite(endcode, 1, sizeof(endcode), fp);
        fclose(fp);

        this->InitializeDecoder();
        for (unsigned int i = 0; i < m_encodedFrames.size(); i++)
        {
            this->AddFrame(&m_encodedFrames[i]);
            //std::cout << m_rawFrames[i-26]->width << ", " << m_rawFrames[i-26]->height << std::endl;
        }
        
        
        //this->AddFrame(&m_encodedFrames[1]);
        
    }

    // Test Encode to File
    void Encode()
    {
        this->InitializeDecoder();
        for (unsigned int i = 0; i < m_encodedFrames.size(); i++)
        {
            this->AddFrame(&m_encodedFrames[i]);
            //std::cout << m_rawFrames[i-26]->width << ", " << m_rawFrames[i-26]->height << std::endl;
        }
        /*FILE* fp = fopen("test.avi", "wb");
        for (int i = 0; i < m_encodedFrames.size(); i++)
        {
            fwrite(m_encodedFrames[i].data, 1, m_encodedFrames[i].size, fp);
        }
        uint8_t endcode[] = { 0, 0, 1, 0xb7 };
        fwrite(endcode, 1, sizeof(endcode), fp);
        fclose(fp);*/
    }
protected:
    
};