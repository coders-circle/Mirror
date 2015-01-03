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
        av_free(m_frame);
    if (m_frameRGB) 
        av_free(m_frameRGB);
    if (m_codecCtx)
        avcodec_close(m_codecCtx);
    if (m_formatCtx)
        avformat_close_input(&m_formatCtx);
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
        throw Exception("Couldn't open input stream");
    if (avformat_find_stream_info(m_formatCtx, NULL) < 0)
        throw Exception("No stream info");
    av_dump_format(m_formatCtx, 0, "/dev/video0", 0);
    
    for (int i = 0; i < m_formatCtx->nb_streams; ++i)
    if (m_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        m_stream = m_formatCtx->streams[i];
        break;
    }
    if (!m_stream)
        throw Exception("No video stream");

    m_codecCtx = m_stream->codec;
    m_codec = avcodec_find_decoder(m_codecCtx->codec_id);
    if (!m_codec)
        throw Exception("No supported codec");
    if (avcodec_open2(m_codecCtx, m_codec, NULL) < 0)
        throw Exception ("Couldn't open codec");

    m_frame = av_frame_alloc();
    m_frameRGB = av_frame_alloc();
    av_init_packet(&m_packet);
    
    AVPixelFormat pFormat = AV_PIX_FMT_RGB24;
    auto numBytes = avpicture_get_size(pFormat, m_codecCtx->width, m_codecCtx->height) ;
    auto buffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture*)m_frameRGB, buffer, pFormat, m_codecCtx->width, m_codecCtx->height);

    VideoStream::InitializeEncoder(m_codecCtx->width, m_codecCtx->height);
    
    m_imgConvertCtx = sws_getCachedContext(NULL, m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,  
                                             m_codecCtx->width, m_codecCtx->height, pFormat, SWS_BICUBIC, NULL, NULL,NULL);
}

void VideoCapture::Record()
{
    while (m_recording && av_read_frame(m_formatCtx, &m_packet)>=0)
    {
        if (m_packet.stream_index != m_stream->index)
            continue;
        int isFrameAvailable=0;
        int len = avcodec_decode_video2(m_codecCtx, m_frame, &isFrameAvailable, &m_packet);
        if (len<0)
            throw Exception("Decoding error");

        if (isFrameAvailable)
        {
            sws_scale(m_imgConvertCtx, ((AVPicture*)m_frame)->data, ((AVPicture*)m_frame)->linesize, 0, m_codecCtx->height, ((AVPicture *)m_frameRGB)->data, ((AVPicture *)m_frameRGB)->linesize);
            VideoStream::AddFrame(m_frameRGB->data[0], m_packet.pts);
            av_free_packet(&m_packet);
        }
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
    StartRecording();
    std::cout << "Enter something to interrupt" << std::endl;
    char c;
    std::cin >> c;
    StopRecording();
    VideoStream::Encode();
}
