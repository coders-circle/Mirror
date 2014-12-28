#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include <iostream>
#include <portaudiocpp/PortAudioCpp.hxx>
#include "AudioBuffer.h"

/*
	Wrapper class for handling raw stream recording and playback
*/

class AudioStream
{
public:
	AudioStream(AudioBuffer & objAudioBuf, portaudio::System & sys) ;
	~AudioStream(void);
	/* For Recording */
	void SetRecordStreamParameters(int numChannels, portaudio::SampleDataFormat format, 
						double sampleRate, unsigned long framesPerBuffer);
	void CreateRecordStream(void);
	void StartRecordStream(void) {	m_streamRecord->start();	}
	void StopRecordStream(void) {	m_streamRecord->stop();	}
	void CloseRecordStream(void) {	m_streamRecord->close();	}


	/* For Playback */
	void SetPlaybackStreamParameters(int numChannels, portaudio::SampleDataFormat format, 
						double sampleRate, int framesPerBuffer);
	void CreatePlaybackStream(void);
	void StartPlaybackStream(void) {	m_streamPlayback->start();	}
	void StopPlaybackStream(void) {	m_streamPlayback->stop();}
	bool IsPlaybackStreamActive(void) const {	return m_streamPlayback->isActive();	}
	void ClosePlaybackStream(void) {	m_streamPlayback->close(); }


private:
	AudioBuffer & m_objAudioBuffer; /* a reference to original audiobuffer object */
	portaudio::System & m_system;	/* a reference to ogirinal initialized portaudio system */

	int m_inputDevice;
	int m_outputDevice;

	/*	RECORD PARAMETERS */
	portaudio::DirectionSpecificStreamParameters m_inParamsRecord;
	portaudio::StreamParameters m_paramsRecord;
	portaudio::MemFunCallbackStream<AudioBuffer> *m_streamRecord;  // this should be only a singleton for whole program

	/* PLAYBACK PARAMETERS */
	portaudio::DirectionSpecificStreamParameters m_outParamsPlayback;
	portaudio::StreamParameters m_paramsPlayback;
	portaudio::MemFunCallbackStream<AudioBuffer> *m_streamPlayback;	 // this should be only a singleton for whole program

};


#endif