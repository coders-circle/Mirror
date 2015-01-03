#pragma once
#include "VideoStream.h"

class VideoCapture : public VideoStream
{
public:
    VideoCapture();

    // Start recording from webcam in separate thread
    void StartRecording();
    // Stop the recording going on separate thread
    void StopRecording();

    // Test function to start recording and stop on input
    void Test();

private:
    AVFormatContext* m_formatCtx;
    AVCodecContext* m_codecCtx;
    AVCodec* m_codec;
    AVStream* m_stream;
    SwsContext* m_imgConvertCtx;
    AVFrame *m_frame, *m_frameRGB;
    AVPacket m_packet;
    boost::thread m_recordThread;
 
    void Initialize();
    void CleanUp();

    bool m_recording;
    void Record();
};
