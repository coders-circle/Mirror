/* Client - main.cpp */

#include <common/common.h>
#include <client/MediaStream/AudioCapture.h>
#include <client/MediaStream/AudioPlayback.h>


//VideoPlayback *v;

//Client client;
int main(int argc, char *argv[])
{
    try
    {
        av_register_all();
        avdevice_register_all();
        avcodec_register_all();
        
        AudioCapture capture;
        AudioPlayback playback;
        capture.SetCaptureCallback([&playback](AVPacket& packet){
            playback.AddFrame(packet);
        });
        playback.StartPlayback();
        capture.StartCapturing();

        return 0;
    }
    catch (std::exception& err)
    {
        std::cout << "\n:/ \n" << err.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "\n:/ \n" << "Unexpected exception occured"  << std::endl;
        return 1;
    }
}


#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "gdk-win32-3.0.lib")
#endif

