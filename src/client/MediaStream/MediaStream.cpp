#include "common/common.h"
#include "client/MediaStream/MediaStream.h"

MediaStream::MediaStream()
{
    m_encoder = 0;
    m_decoder = 0;
    m_encoderContext = 0;
    m_decoderContext = 0;
}

MediaStream::~MediaStream()
{
    if (m_encoderContext)
    {
       // avcodec_free_context(&m_encoderContext);
        avcodec_close(m_encoderContext);
        av_freep(&m_encoderContext->extradata);
        av_freep(&m_encoderContext);
    }
    if (m_decoderContext)
    {
        //avcodec_free_context(&m_decoderContext);
        avcodec_close(m_decoderContext);
        av_freep(&m_decoderContext->extradata);
        av_freep(&m_decoderContext);
    }
}

unsigned int MediaStream::AllocateNewEndodedPacket()
{
    m_encodedPackets.resize(m_encodedPackets.size() + 1);
    return (m_encodedPackets.size() - 1);
}

unsigned int MediaStream::AllocateNewDecodedFrame()
{
    m_decodedFrames.resize(m_decodedFrames.size() + 1);
    return (m_decodedFrames.size() - 1);
}

void MediaStream::EraseEncodedPacketFromHead(unsigned int noOfPackets)
{
    m_encodedPackets.erase(m_encodedPackets.begin(), m_encodedPackets.begin() + noOfPackets);
}

void MediaStream::EraseDecodedFrameFromHead(unsigned int noOfFrames)
{
    m_decodedFrames.erase(m_decodedFrames.begin(), m_decodedFrames.begin() + noOfFrames);
}

unsigned int MediaStream::AllocateRawData(unsigned int noOfBytesToAdd)
{
    m_rawData.resize(m_rawData.size() + noOfBytesToAdd);
    return (m_rawData.size() - noOfBytesToAdd);
}
void MediaStream::EraseRawDataFromHead(unsigned int noOfBytesToRemove)
{
    m_rawData.erase(m_rawData.begin(), m_rawData.begin() + noOfBytesToRemove);
}


void MediaStream::SetupEncoder()
{
    m_encoder = avcodec_find_encoder(m_codecID);
    if (!m_encoder)
    {
        throw CodecNotFound();
    }
    m_encoderContext = avcodec_alloc_context3(m_encoder);
    if (!m_encoderContext)
    {
        throw Exception("can't allocate the encoder context");
    }
}

void MediaStream::SetupDecoder()
{
    m_decoder = avcodec_find_decoder(m_codecID);
    if (!m_decoder)
    {
        throw CodecNotFound();
    }
    m_decoderContext = avcodec_alloc_context3(m_decoder);
    if (!m_decoderContext)
    {
        throw Exception("can't allocate the decoder context");
    }
}

void MediaStream::OpenCodec(AVCodecContext* codecContext, AVCodec* codec)
{
    if (avcodec_open2(codecContext, codec, NULL) < 0)
    {
        throw Exception("failed to open the codec");
    }
}

unsigned int MediaStream::AllocateEncodedDataStream(unsigned int noOfBytesToAdd)
{
    m_encodedDataStream.resize(m_encodedDataStream.size() + noOfBytesToAdd);
    return (m_encodedDataStream.size() - noOfBytesToAdd);
}

void MediaStream::EraseEncodedDataStreamFromHead(unsigned int noOfBytesToRemove)
{
    m_encodedDataStream.erase(m_encodedDataStream.begin(), m_encodedDataStream.begin() + noOfBytesToRemove);
}

void MediaStream::AddEncodedDataStream(unsigned char* encodedStream, unsigned int streamSizeInBytes)
{
    int encodedDataStreamIndex = this->AllocateEncodedDataStream(streamSizeInBytes);
    memcpy(m_encodedDataStream.data() + encodedDataStreamIndex, encodedStream, streamSizeInBytes);
}



#ifdef _WIN32
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#endif
