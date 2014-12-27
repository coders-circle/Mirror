#include "client/Audio/AudioStream.h"


AudioStream::AudioStream(AudioBuffer & objAudioBuffer, portaudio::System & sys) : 
							m_objAudioBuffer(objAudioBuffer), m_system(sys), m_streamRecord(NULL), m_streamPlayback(NULL)
{
	m_inputDevice	= m_system.defaultInputDevice().index();
	m_outputDevice	= m_system.defaultOutputDevice().index();

}

AudioStream::~AudioStream(void)
{
	delete m_streamRecord;
	delete m_streamPlayback;
}

/* --------------------------- RECORD ----------------------------------------- */

/* Set Record Stream Parameters using privided params */
void AudioStream::SetRecordStreamParameters(int numChannels, portaudio::SampleDataFormat format, 
											double sampleRate, unsigned long framesPerBuffer)
{
	//std::cout << m_system.deviceByIndex(m_inputDevice).name() << std::endl;	
	portaudio::DirectionSpecificStreamParameters inParamsRecord(m_system.deviceByIndex(m_inputDevice), 
																	numChannels, 
																	portaudio::INT16, 
																	false, 
																	m_system.deviceByIndex(m_inputDevice).defaultLowInputLatency(), 
																	NULL);
	m_inParamsRecord = inParamsRecord;


	portaudio::StreamParameters paramsRecord(m_inParamsRecord, 
											portaudio::DirectionSpecificStreamParameters::null(), 
											sampleRate, 
											framesPerBuffer, 
											paClipOff);
	m_paramsRecord = paramsRecord;
}

/* now create record stream
	m_streamRecord must be a singleton object
	in the entire progra.
	so create it only once.

	m_streamRecord is the pointer to the member function callback for record
	which is only called once in an automated  callback thread
	unless destroyed and re-created
	:P
 */
void AudioStream::CreateRecordStream(void)
{
	if(!m_streamRecord)
	{
		m_streamRecord = new portaudio::MemFunCallbackStream<AudioBuffer> (m_paramsRecord, m_objAudioBuffer, &AudioBuffer::RecordCallback);
	}
}

/* ----------------	For Playback -------------------------------------------------- */

/* Set stream parameters for playback */
void AudioStream::SetPlaybackStreamParameters(int numChannels, portaudio::SampleDataFormat format, 
											double sampleRate, int framesPerBuffer)
{
	//std::cout << m_system.deviceByIndex(m_outputDevice).name() << std::endl;
	portaudio::DirectionSpecificStreamParameters inParamsPlayback(m_system.deviceByIndex(m_outputDevice), 
																	numChannels, 
																	portaudio::INT16, 
																	false, 
																	m_system.deviceByIndex(m_outputDevice).defaultLowInputLatency(), 
																	NULL);
	m_outParamsPlayback = inParamsPlayback;

	portaudio::StreamParameters paramsPlayback(portaudio::DirectionSpecificStreamParameters::null(), 
											m_outParamsPlayback, 
											sampleRate, 
											framesPerBuffer, 
											paClipOff);
	m_paramsPlayback = paramsPlayback;
}

/* now create playback stream
	m_streamPlayback must be a singleton object
	in the entire progra.
	so create it only once.

	m_streamPlayback is the pointer to the member function callback for playback
	which is only called once in an automated  callback thread
 */
void AudioStream::CreatePlaybackStream(void)
{
	
	if(!m_streamPlayback)
	{
		m_streamPlayback = new portaudio::MemFunCallbackStream<AudioBuffer> (m_paramsPlayback, m_objAudioBuffer, &AudioBuffer::PlaybackCallback);
	}
}


