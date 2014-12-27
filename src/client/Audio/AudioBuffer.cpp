#include "client/Audio/AudioBuffer.h"

/*
	Constructor, caller can give us a hint about the number of samples we may need to hold.
	allocate the frame samples
*/
AudioBuffer::AudioBuffer(int sizeHint)
{
	std::cout << "AudioBuffer Constructor" << std::endl;

	if(sizeHint)
		m_vectorSamples.reserve(sizeHint * 2);

	m_iterPlayback = m_vectorSamples.begin();
}

AudioBuffer::~AudioBuffer(void)
{

}

/*
	For Recording the input.
	Input data gets stored as frame samples in m_vectorsamples
*/
int AudioBuffer::RecordCallback(const void *inputBuffer,
								void * outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags)
{
	if(!inputBuffer)
	{
		std::cout << "AudioBuffer::RecordCallback --> inputBuffer is NULL" << std::endl;
		return paContinue;
	}

	short **data = (short**) inputBuffer;

	// Copy all the frames over to our internal vector of samples
	for (unsigned long i = 0; i < framesPerBuffer; i++)
		m_vectorSamples.push_back(data[0][i]);

	return paContinue;
}


int AudioBuffer::PlaybackCallback(const void *inputBuffer,
								void * outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags)
{
	if(!outputBuffer)
	{
		std::cout << "AudioBuffer::PlayCallback --> NULL" << std::endl;
		return paContinue;
	}

	short** data = (short**) outputBuffer;
	unsigned long output = 0;

	// Output samples until we either have satified the caller, or we run out
	while(output < framesPerBuffer)
	{
		if(m_iterPlayback == m_vectorSamples.end())
		{
			// Fill out buffer with zeros
			while(output < framesPerBuffer)
			{
				data[0][output] = static_cast<short>(0);
				output++;
			}
			return paComplete;
		}

		data[0][output] = (short) *m_iterPlayback;

		m_iterPlayback++;
		output++;
	}

	return paContinue;
}

/* Clear the whole audio buffer */
void AudioBuffer::ClearBuffer(void)
{
	m_vectorSamples.clear();
}

/*just resets the playback index to start of the buffer*/
void AudioBuffer::ResetPlayback(void)
{
	m_iterPlayback = m_vectorSamples.begin();
}

// Dump the samples to a raw file 
void AudioBuffer::WriteToFile(const std::string & strFilename)
{
	std::fstream fout(strFilename.c_str(), std::ios::out|std::ios::binary);

	for (std::vector<short>::iterator iter = m_vectorSamples.begin(); iter != m_vectorSamples.end(); iter++)
	{
		short sample = (short) *iter;
		fout.write((char *) &sample, sizeof(short));
	}
	
	fout.close();
}