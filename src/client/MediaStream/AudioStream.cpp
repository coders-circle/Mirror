//#include "common/common.h"
//#include "client/MediaStream/AudioStream.h"
//
//AudioStream::AudioStream()
//{
//    m_codecID = AV_CODEC_ID_AAC;
//}
//
//void AudioStream::InitializeEncoder(int samplerate, int bitrate, int channel)
//{
//    this->SetupEncoder();
//    AVRational avrat = { 1, samplerate };
//
//    m_encoderContext->bit_rate = bitrate;
//    m_encoderContext->sample_rate = samplerate;
//    m_encoderContext->channels = channel;
//    m_encoderContext->sample_fmt = AV_SAMPLE_FMT_S16;
//    m_encoderContext->time_base = avrat;
//    m_encoderContext->codec_type = AVMEDIA_TYPE_AUDIO ;
//    m_encoderContext->channel_layout = (channel == 1 ) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
//    m_encoderContext->profile = FF_PROFILE_AAC_LOW;
//
//    this->OpenCodec(m_encoderContext, m_encoder);
//}
//
//void AudioStream::AddFrame(uint8_t* samples, int pts)
//{
//	AVFrame *frame = av_frame_alloc();  // frame containning the input raw sample
//
//	frame->nb_samples = m_encoderContext->frame_size;
//    frame->format = m_encoderContext->sample_fmt;
//    frame->channel_layout = m_encoderContext->channel_layout;
//
//    // the codec gives us the frame size, in the sample
//    // we calculate the size of the samples buffer in bytes
//    int bufferSize = av_samples_get_buffer_size(NULL, m_encoderContext->channels, 
//    						m_encoderContext->frame_size, m_encoderContext->sample_fmt, 0);
//    if(bufferSize < 0)
//    {
//        throw Exception("could not get sample buffer size");
//    }
//
//    if(avcodec_fill_audio_frame(frame, m_encoderContext->channels, m_encoderContext->sample_fmt, 
//                                (const uint8_t*)samples, bufferSize, 0) < 0)
//    {
//        throw Exception("failed to allocate raw sample buffer");   
//    }
//
//    frame->pts = pts;   
//    this->AddFrame(frame);
//}
//
//void AudioStream::AddFrame(AVFrame *frame)
//{
//    unsigned int packetIndex = this->AllocateNewEndodedPacket();
//    m_encodedPackets[packetIndex] = new AVPacket;
//    AVPacket* pkt = m_encodedPackets[packetIndex];
//
//    av_init_packet(pkt);
//    pkt->data = NULL;
//    pkt->size = 0;
//
//    int packetFilled = 0;
//    if(avcodec_encode_audio2(m_encoderContext, pkt, frame, &packetFilled) < 0)
//    {
//        throw FailedToEncode();
//    }
//}
//
//void AudioStream::Test(void)
//{
//    this->InitializeEncoder();
//    uint8_t* samples = new uint8_t[m_encoderContext->frame_size*2*m_encoderContext->channels];
//
//    float freq = 500;
//    float cycleInc = static_cast<float>(freq / this->m_encoderContext->sample_rate);
//    float cyclePos = 0.0f;
//
//
//    float t=0.0f;
//    float tincr = 1.0f;
//    int j;
//    for(int i=0; i<200; i++)
//    {
//        for(j=0; j<m_encoderContext->frame_size; j++)
//        {
//            samples[2*j] = static_cast<int>(0.5f + 99.0f*sin(2*3.14159f*cyclePos));
//            samples[2*j+1] = samples[2*j];
//            t += tincr;
//            cyclePos += cycleInc;
//            if (cyclePos > 1.0f)
//                cyclePos -= 1.0f;
//        }
//        this->AddFrame(samples, i);
//    }
//
//    FILE* fp = fopen("encodetest.aac", "wb");
//    for (int unsigned i = 0; i < m_encodedPackets.size(); i++)
//    {
//         fwrite(m_encodedPackets[i]->data, 1, m_encodedPackets[i]->size, fp);
//    }
//    fclose(fp);
//
//}