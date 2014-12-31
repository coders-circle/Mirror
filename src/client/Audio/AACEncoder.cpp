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
	/*
	std::vector<short> samples = objAudioBuffer.GetSamples();
	size_t rawSize = sizeof(short) * samples.size();
	std::cout << "raw size in bytes: " << rawSize << std::endl;
	*/
	

	FILE *stream = fopen("rawaudio", "rb");
    size_t audioSize;
    uint8_t *rawAudio;

    if (!stream)
    {
        std::cout << "Unable to open file" << std::endl;
        return false;
    }

    fseek(stream, 0, SEEK_END);
    audioSize = ftell(stream);
    //std::cout<<size<<std::endl;
    fseek(stream, 0, SEEK_SET);
    rawAudio = (uint8_t *)malloc(audioSize);
    fread(rawAudio, sizeof(uint8_t), audioSize, stream);
    fclose(stream);

    int outBufsize = FF_MIN_BUFFER_SIZE * 10;
	//uint8_t *outbuf = new uint8_t[outBufsize];
	uint8_t* outbuf = (uint8_t *)malloc(outBufsize);
	if (!outbuf) 
		return false;

	int frameBytes = m_codecContext->frame_size * m_codecContext->channels * sizeof(short);

	std::fstream fout("encoded", std::ios::out|std::ios::binary);

	while (audioSize >= frameBytes)
	{
		int packetSize = avcodec_encode_audio(m_codecContext, outbuf, outBufsize, (short *)rawAudio);
		rawAudio += frameBytes;
        fout.write((char *) outbuf,packetSize);
        audioSize -= frameBytes;
	}

	fout.close();
	//free(rawAudio);
	//delete [] outbuf;
}