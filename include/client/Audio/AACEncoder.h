#ifndef AACENCODER_H
#define AACENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "AudioBuffer.h"

extern "C" { 
#include <libavutil/channel_layout.h>
#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h>
}

/*
	Just an AAC encoding class 
*/

class AACEncoder
{
public:
	AACEncoder(int bitrate = 24000, int samplerate = 16000, int channel = 1);
	~AACEncoder(void);

	bool Initialize(void);
	bool Encode(const AudioBuffer & objAudioBuffer);
	void Close(void);

private:
	AVCodec *m_codec;
	AVCodecContext *m_codecContext;


	int m_bitRate;
	int m_sampleRate;
	int m_channelNum;
};

#endif //AACENCODER_H