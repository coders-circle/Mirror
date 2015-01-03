#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>

#include <portaudiocpp/PortAudioCpp.hxx>

/*
	This is the class for holding everything about our audio.



*/

class AudioBuffer
{
public:
	AudioBuffer(int sizeHint);
	~AudioBuffer(void);

	/* The callback functoin for recording  */
	int RecordCallback(const void *inputBuffer,
						void * outputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags);

	/* callback function for playing back the recorded data*/
	int PlaybackCallback(const void *inputBuffer,
						void * outputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags);

	/* clears the input buffer before starting new recording*/
	void ClearBuffer(void);

	/* resets the buffer playback */
	void ResetPlayback(void);

	void WriteToFile(const std::string & strFilename);

	std::vector<short> GetSamples(void) const { return m_vectorSamples; }
	
private:
	std::vector<short> m_vectorSamples;
	std::vector<short>::iterator m_iterPlayback;
};

#endif
