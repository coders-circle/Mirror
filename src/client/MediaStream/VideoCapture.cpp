#include <common/common.h>
#include <client/MediaStream/VideoCapture.h>

void VideoCapture::Initialize()
{
   
    AVFormatContext* avFormat = avformat_alloc_context();
#ifdef _WIN32
    AVInputFormat * ifmt = av_find_input_format("dshow");
    if (avformat_open_input(&avFormat, "video=Integrated Webcam", ifmt, NULL)!=0)
#else
    AVInputFormat * ifmt = av_find_input_format("video4linux2");
    if (avformat_open_input(&avFormat, "/dev/video0", ifmt, NULL)!=0)
#endif
        throw Exception("Couldn't open input stream");
        
    if (avformat_find_stream_info(avFormat, NULL) < 0)
        throw Exception("No stream info");
        
    av_dump_format(avFormat, 0, "/dev/video0", 0);
    AVStream * stream = NULL;
    for (int i = 0; i < avFormat->nb_streams; ++i)
    if (avFormat->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        stream = avFormat->streams[i];
        break;
    }
     if (!stream)
        throw Exception("No video stream");
     auto avCodec = stream->codec;
    auto codec = avcodec_find_decoder(avCodec->codec_id);
    if (!codec)
        throw Exception("No supported codec");
     
    if (avcodec_open2(avCodec, codec, NULL) < 0)
        throw Exception ("Couldn't open codec");
    AVFrame * frame;
    AVFrame * frameRGB = av_frame_alloc();
    AVPacket pkt;
    av_init_packet(&pkt);
    frame = av_frame_alloc();
    AVPixelFormat  pFormat = AV_PIX_FMT_RGB24;
    auto numBytes = avpicture_get_size(pFormat,avCodec->width,avCodec->height) ;
    auto buffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture *) frameRGB,buffer,pFormat,avCodec->width,avCodec->height);

    bool keepgoing = true;
    bool * kg = &keepgoing;
    boost::thread t([kg](){
        char c;
        std::cout << "Enter"; std::cin >> c;
        *kg=false;
    });
                 
    VideoStream::InitializeEncoder(avCodec->width, avCodec->height);
    while (keepgoing && av_read_frame(avFormat, &pkt)>=0)
    {
        if (pkt.stream_index != stream->index)
            continue;
        int isFrameAvailable=0;
        int len = avcodec_decode_video2(avCodec, frame, &isFrameAvailable,&pkt);
        if (len<0)
            throw Exception("Decoding error");
        if (isFrameAvailable)
        {
            struct SwsContext * img_convert_ctx;
            img_convert_ctx = sws_getCachedContext(NULL,avCodec->width, avCodec->height, avCodec->pix_fmt,   avCodec->width, avCodec->height, pFormat, SWS_BICUBIC, NULL, NULL,NULL);
            sws_scale(img_convert_ctx, ((AVPicture*)frame)->data, ((AVPicture*)frame)->linesize, 0, avCodec->height, ((AVPicture *)frameRGB)->data, ((AVPicture *)frameRGB)->linesize);
            sws_freeContext(img_convert_ctx);
            VideoStream::AddFrame(frameRGB->data[0], pkt.pts);
            av_free_packet(&pkt);
        }
    }
    av_free_packet(&pkt);
    av_free(frame);
    av_free(frameRGB);
    avcodec_close(avCodec);
    avformat_close_input(&avFormat);

    VideoStream::Encode();
}
