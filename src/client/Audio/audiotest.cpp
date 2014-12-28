#include "client/Audio/AudioBuffer.h"
#include "client/Audio/AudioStream.h"

#include <iostream>
#include <exception>
#include <cassert>

// Some constants:
const double SAMPLE_RATE = 16000.0;
const unsigned long FRAMES_PER_BUFFER	= 64;
const int NUM_CHANNELS = 1;

int main()
{
	try
	{
		/* Create a object that is used to record, save to file and play the audio. */
		AudioBuffer	objAudioBuffer((int) (SAMPLE_RATE * 60));

		/* Set up the System: */
		portaudio::AutoSystem autoSys;
		portaudio::System &sys = portaudio::System::instance();


		AudioStream audioStream(objAudioBuffer, sys);


		audioStream.SetRecordStreamParameters(NUM_CHANNELS, portaudio::INT16, SAMPLE_RATE, FRAMES_PER_BUFFER);
		audioStream.CreateRecordStream();

		/* create playback stream */
		//std::cout << "Opening playback output stream on " << sys.deviceByIndex(outputDevice).name() << std::endl;
		audioStream.SetPlaybackStreamParameters(NUM_CHANNELS, portaudio::INT16, SAMPLE_RATE, FRAMES_PER_BUFFER);
		audioStream.CreatePlaybackStream();

		std::cout << "input and output stream are all set..." << std::endl;
		std::cout << "press ENTER to start to start recording" << std::endl;
		char cWait;
		std::cin.get(cWait);
		std::cout << "recording started.. press ENTER to stop recording" << std::endl;

		audioStream.StartRecordStream();
		std::cin.get(cWait);
		//sys.sleep(1000);
		audioStream.StopRecordStream();

		std::cout << "recording successful... Press ENTER to play the audio" << std::endl;
		//std::cin.get(cWait);

		objAudioBuffer.WriteToFile("audio.raw");

		std::cout << "Playing back samples." << std::endl;
		objAudioBuffer.ResetPlayback();

		audioStream.StartPlaybackStream();
		while (audioStream.IsPlaybackStreamActive())
			sys.sleep(100);
		audioStream.StopPlaybackStream();

		// Close the Stream (not strictly needed as terminating the System will also close all open Streams):
		audioStream.CloseRecordStream();
		audioStream.ClosePlaybackStream();

		std::cout << "so far so good" << std::endl;

		sys.terminate();
	}

	catch (const portaudio::PaException &e)
	{
		std::cout << "A PortAudio error occured: " << e.paErrorText() << std::endl;
	}

	catch (const portaudio::PaCppException &e)
	{
		std::cout << "A PortAudioCpp error occured: " << e.what() << std::endl;
	}

	catch (...)
	{
		std::cout << "An unknown exception occured." << std::endl;
	}

	return 0;
}