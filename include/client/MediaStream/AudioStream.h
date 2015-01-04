#pragma once

#include "client/MediaStream/MediaStream.h"



class AudioStream : public MediaStream
{
public:
    AudioStream();

    // Initialize the audio stream for encoding process
    void InitializeEncoder(int samplerate = 16000, int bitrate=24000, int channel=2);

    // Add raw data and encode it into the packet
    void AddFrame(uint8_t* rawData, int pts);

    // Add raw frame and encode it into the packet
    void AddFrame(AVFrame *frame);

    // @@@@@@@@@@@@@@@@@
    // TEMPORARY function
    // test for the audio encoding
    void Test(void);

protected:
private:

};
