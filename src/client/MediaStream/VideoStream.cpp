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

    // for progressive decoding
    /*if (m_decoder->capabilities & CODEC_CAP_TRUNCATED)
        m_decoderContext->flags |= CODEC_FLAG_TRUNCATED;*/

    this->OpenCodec(m_decoderContext, m_decoder);
}
// do not use!
// under construction
//int VideoStream::AddProgressivePacket(AVPacket* pkt)
//{
//    AVFrame* frame = av_frame_alloc();
//    int framePresent = 0;
//    int readBytes = avcodec_decode_video2(m_decoderContext, frame, &framePresent, pkt);
//    if (readBytes < 0)
//    {
//        throw FailedToDecode();
//    }
//    if (framePresent && frame->width != 0)
//    {
//        unsigned int frameIndex = this->AllocateNewDecodedFrame();
//        m_decodedFrames[frameIndex] = av_frame_clone(frame);
//        // av_frame_clone will not allocate the packet if the frame is empty
//        // so we have to check for that and allocate manually for empty packets
//        if (!m_decodedFrames[frameIndex])
//        {
//            m_decodedFrames[frameIndex] = av_frame_alloc();
//        }
//        
//        av_frame_copy(m_decodedFrames[frameIndex], frame);
//        if (m_rawData.size() == 0)
//        {
//            // this is the first time we get the filled frame
//            // so allocate enough memory for raw data and
//            // initialize scaler context for color fromat conversion
//            this->AllocateRawData(frame->height*frame->width * 3);
//            if (m_YUV420PToRGB24ConverterContext)
//            {
//                //should not happen
//                sws_freeContext(m_YUV420PToRGB24ConverterContext);
//            }
//            // @@@@@@@@@@@@@@@@@@@@@
//            // zzz encoder context!!!
//            m_YUV420PToRGB24ConverterContext = sws_getContext(m_encoderContext->width,
//                m_encoderContext->height, AV_PIX_FMT_YUV420P, m_encoderContext->width,
//                m_encoderContext->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
//        }
//    }
//    return readBytes;
//}

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
            unsigned int frameIndex = this->AllocateNewDecodedFrame();
            m_decodedFrames[frameIndex] = av_frame_clone(frame);
            // av_frame_clone will not allocate the packet if the frame is empty
            // so we have to check for that and allocate manually for empty packets
            if (!m_decodedFrames[frameIndex])
            {
                m_decodedFrames[frameIndex] = av_frame_alloc();
            }
            av_frame_copy(m_decodedFrames[frameIndex], frame);
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
                m_fps = m_decoderContext->framerate.num;
                m_YUV420PToRGB24ConverterContext = sws_getContext(m_fw, m_fh, AV_PIX_FMT_YUV420P, 
                    m_fw, m_fh, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
            }
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
    m_encoderContext->gop_size = 10;
    m_encoderContext->max_b_frames = 1;
    m_encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (m_codecID == AV_CODEC_ID_H264)
    {
        av_opt_set(m_encoderContext->priv_data, "preset", "slow", 0);
    }
    this->OpenCodec(m_encoderContext, m_encoder);

    m_RGB24ToYUP420PConverterContext = sws_getContext(m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_RGB24, m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);

    m_fw = w;
    m_fh = h;
    m_fps = fps;
}

void VideoStream::AddFrame(uint8_t* rgb24Data, int pts)
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
        throw FailedToEncode();
    }
}