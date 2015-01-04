#pragma once



#include "client/MediaStream/MediaStream.h"

#ifndef _WIN32
#define av_frame_alloc avcodec_alloc_frame
#endif




class AudioStream : public MediaStream
{
public:
    void Initialize(int samplerate = 16000, int bitrate=24000, int channel=2);

    unsigned int AllocateNewPacket(void);

    void AddFrame(uint8_t* rawData, int pts);
    void AddFrame(AVFrame *frame);

    void Test(void);

protected:
    AVCodecID m_codecID;
    AVCodec *m_codec;
    AVCodecContext *m_encoderContext;
    std::vector<AVPacket> m_encodedFrames;

private:

};
