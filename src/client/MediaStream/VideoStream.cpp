#include "common/common.h"
#include "client/MediaStream/VideoStream.h"
//
//
//void VideoStream::Test()
//{
//    this->InitializeEncoder(640, 480, 30, 400000);
//    int w = m_encoderContext->width;
//    int h = m_encoderContext->height;
//    
//    boost::thread memLeakTestThread([this, w, h]()
//    {
//        uint8_t *rgb24Data = new uint8_t[w*h * 3];
//        unsigned long long pts = 0;
//        while (1)
//        {
//            for (int y = 0; y < h; y++)
//            {
//                for (int x = 0; x < w; x++)
//                {
//                    rgb24Data[3 * (y*w + x) + 0] = 255.0*abs(sin(pts*3.14159f / 360.0f));
//                    rgb24Data[3 * (y*w + x) + 1] = 255.0*abs(sin(x*3.14159f / 800.0f));
//                    rgb24Data[3 * (y*w + x) + 2] = 255.0*abs(sin(y*3.14159f / 480.0f));
//                    
//                }
//            }
//            this->EncodeToPacket(rgb24Data, pts++);
//            this->newAvailable = true;
//            boost::this_thread::sleep(boost::posix_time::milliseconds(50));
//        }
//        
//    });
//}


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
        av_opt_set(m_encoderContext->priv_data, "tune", "zerolatency", 0);
    }
    this->OpenCodec(m_encoderContext, m_encoder);

    m_RGB24ToYUP420PConverterContext = sws_getContext(m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_RGB24, m_encoderContext->width,
        m_encoderContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);

    //std::cout << m_encoderContext->width << std::endl << m_encoderContext->height << std::endl;

    m_fw = w;
    m_fh = h;
    m_fps = fps;
}
