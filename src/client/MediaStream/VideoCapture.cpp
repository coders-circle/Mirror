#include <common/common.h>
#include <client/MediaStream/VideoCapture.h>

VideoCapture::VideoCapture()
: m_formatCtx(NULL), m_codecCtx(NULL), m_codec(NULL), m_stream(NULL), m_imgConvertCtx(NULL),
  m_frame(NULL), m_frameRGB(NULL)
{}

void VideoCapture::CleanUp()
{
    av_free_packet(&m_packet);
    if (m_frame)
    {
        av_freep(&m_frame->data[0]);
        av_frame_free(&m_frame);
    }
    if (m_frameRGB)
    {
        av_freep(&m_frameRGB->data[0]);
        av_frame_free(&m_frameRGB);
    }
    /*if (m_codecCtx)
        avcodec_close(m_codecCtx);
    if (m_formatCtx)
        avformat_close_input(&m_formatCtx);*/
    if (m_imgConvertCtx)
        sws_freeContext(m_imgConvertCtx);
}

void VideoCapture::Initialize()
{
    m_formatCtx = avformat_alloc_context();
#ifdef _WIN32
    AVInputFormat * ifmt = av_find_input_format("dshow");
    if (avformat_open_input(&m_formatCtx, "video=Integrated Webcam", ifmt, NULL)!=0)
#else
    AVInputFormat * ifmt = av_find_input_format("video4linux2");
    if (avformat_open_input(&m_formatCtx, "/dev/video0", ifmt, NULL)!=0)
#endif
        throw VideoCaptureException("Couldn't open input stream");
    if (avformat_find_stream_info(m_formatCtx, NULL) < 0)
        throw VideoCaptureException("No stream info");
    av_dump_format(m_formatCtx, 0, "/dev/video0", 0);
    
    for (unsigned int i = 0; i < m_formatCtx->nb_streams; ++i)
    if (m_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        m_stream = m_formatCtx->streams[i];
        break;
    }
    if (!m_stream)
        throw VideoCaptureException("No video stream");

    m_codecCtx = m_stream->codec;
    m_codec = avcodec_find_decoder(m_codecCtx->codec_id);
    if (!m_codec)
        throw VideoCaptureException("No supported codec");
    if (avcodec_open2(m_codecCtx, m_codec, NULL) < 0)
        throw VideoCaptureException ("Couldn't open codec");

    m_frame = av_frame_alloc();
    m_frameRGB = av_frame_alloc();
    av_init_packet(&m_packet);

    //AVPixelFormat pFormat = AV_PIX_FMT_RGB24;
    AVPixelFormat pFormat = AV_PIX_FMT_YUV420P;
    /*auto numBytes = avpicture_get_size(pFormat, m_codecCtx->width, m_codecCtx->height) ;
    auto buffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture*)m_frameRGB, buffer, pFormat, m_codecCtx->width, m_codecCtx->height);*/

    int w = m_codecCtx->width;
    int h = m_codecCtx->height;

    if (w % 2 != 0) --w;
    if (h % 2 != 0) --h;

    if (av_image_alloc(m_frameRGB->data, m_frameRGB->linesize, w, h, pFormat, 32) < 0)
    {
        throw Exception("failed to allocate raw picture buffer");
    }
    VideoStream::InitializeEncoder(w, h, 15, 200000);
    m_frameRGB->width = m_encoderContext->width;
    m_frameRGB->height = m_encoderContext->height;
    m_frameRGB->format = pFormat;
    m_imgConvertCtx = sws_getCachedContext(NULL, m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,  
                                             w, h, pFormat, SWS_BICUBIC, NULL, NULL, NULL);
}

void VideoCapture::Record()
{
    try
    {
        while (m_recording && av_read_frame(m_formatCtx, &m_packet)>=0)
        {
            if (m_packet.stream_index != m_stream->index)
                continue;
            int isFrameAvailable=0;
            int len = avcodec_decode_video2(m_codecCtx, m_frame, &isFrameAvailable, &m_packet);
            if (len < 0)
                throw FailedToDecode();
    
            if (isFrameAvailable)
            {
                sws_scale(m_imgConvertCtx, ((AVPicture*)m_frame)->data, ((AVPicture*)m_frame)->linesize, 
                    0, m_codecCtx->height, ((AVPicture *)m_frameRGB)->data, ((AVPicture *)m_frameRGB)->linesize);
                //VideoStream::AddFrame(m_frameRGB->data[0], m_packet.pts);
                /*m_frameRGB->width = m_encoderContext->width;
                m_frameRGB->height = m_encoderContext->height;*/
                VideoStream::AddFrame(m_frameRGB);
                av_free_packet(&m_packet);
            }

            boost::this_thread::sleep(boost::posix_time::milliseconds(20));
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

void VideoCapture::StartRecording()
{
    Initialize();
    m_recording = true;
    m_recordThread = boost::thread(boost::bind(&VideoCapture::Record, this));
}

void VideoCapture::StopRecording()
{
    if (!m_recording) 
        return;
    m_recording = false;
    m_recordThread.join();
    CleanUp();
}

void VideoCapture::Test()
{
    /*StartRecording();
    std::cout << "Enter something to interrupt" << std::endl;
    char c;
    std::cin >> c;
    StopRecording();
    VideoStream::Encode();*/
}
