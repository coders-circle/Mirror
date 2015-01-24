#pragma once
#include "VideoStream.h"

class VideoCaptureException : public Exception
{
public:
    VideoCaptureException(const std::string &errorString)
    : Exception("Error capturing video: " + errorString)
    {}
};

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

    void SendRtp(RtpStreamer& streamer, const udp::endpoint& remoteEndpoint, uint32_t sourceId);


private:
    AVFormatContext* m_formatCtx;
    AVCodecContext* m_codecCtx;
    AVCodec* m_codec;
    AVStream* m_stream;
    SwsContext* m_imgConvertCtx;
    AVFrame *m_frame, *m_frameRGB;
    AVPacket m_packet;
    boost::thread m_recordThread;

    uint16_t m_sendSequence;
 
    void Initialize();
    void CleanUp();

    bool m_recording;
    void Record();

    bool m_readyToSend;
    bool m_packetAvailable;

    bool m_cameraAvailable;

};
