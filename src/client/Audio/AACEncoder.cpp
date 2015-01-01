#include "client/Audio/AACEncoder.h"

AACEncoder::AACEncoder(int bitrate, int samplerate, int channel) : m_bitRate(bitrate), m_sampleRate(samplerate),m_channelNum(channel),
														m_codec(NULL), m_codecContext(NULL)
{
	avcodec_register_all();  
    av_register_all();

    Initialize();
}

AACEncoder::~AACEncoder(void)
{
	//Close();
}

void AACEncoder::Close(void)
{
	avcodec_close(m_codecContext);
    av_free(m_codecContext);
}

bool AACEncoder::Initialize(void)
{
	if(!m_codec)
		m_codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if(!m_codec)
	{
		std::cout << "cannot find the encoder" << std::endl;
		return false;
	}

	if(!m_codecContext)
		m_codecContext = avcodec_alloc_context3(m_codec);

	m_codecContext->bit_rate = m_bitRate;
    m_codecContext->sample_rate = m_sampleRate;
    m_codecContext->channels = m_channelNum;
    m_codecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    m_codecContext->time_base = (AVRational){1, m_sampleRate};
    m_codecContext->codec_type = AVMEDIA_TYPE_AUDIO ;
    m_codecContext->channel_layout = (m_channelNum<2 ) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
    m_codecContext->profile = FF_PROFILE_AAC_LOW;

    if(avcodec_open2(m_codecContext, m_codec,NULL) != 0)
    {
        std::cout << "cannot open codec" << std::endl;
        return false;
    }

    return true;   
}

bool AACEncoder::Encode(const AudioBuffer & objAudioBuffer)
{
    short* rawSamples = &(objAudioBuffer.GetSamples())[0];
    size_t rawSize = sizeof(short) * objAudioBuffer.GetSamples().size();

    int outBufsize = FF_MIN_BUFFER_SIZE * 10;
	//uint8_t *outbuf = new uint8_t[outBufsize];
	uint8_t* outbuf = (uint8_t *)malloc(outBufsize);
	if (!outbuf) 
		return false;

	int frameBytes = m_codecContext->frame_size * m_codecContext->channels * sizeof(short);

	std::fstream fout("encoded", std::ios::out|std::ios::binary);

	while (rawSize >= frameBytes)
	{
		int packetSize = avcodec_encode_audio(m_codecContext, outbuf, outBufsize, (short *)rawSamples);
		rawSamples += frameBytes;
        fout.write((char *) outbuf,packetSize);
        rawSize -= frameBytes;
	}

	fout.close();
	//free(rawAudio);
	//delete [] outbuf;
}