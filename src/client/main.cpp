/* Client - main.cpp */

#include <common/common.h>

#include "client/Application.h"
#include "client/MediaStream/VideoCapture.h"
#include "client/MediaStream/AudioStream.h"
#include "client/MediaStream/VideoPlayback.h"

gint TimeoutCallback(gpointer data)
{
    gtk_widget_queue_draw((GtkWidget*)data);
    return TRUE;
}

int main(int argc, char *argv[])
{
    try
    {
        Client client;
        // Test MediaStreamer for streaming media
        std::string ip = "127.0.0.1";
        std::cout << "Enter server ip: ";
        std::cin >> ip;

        MediaStreamer mediaStreamer;
        // Set callbacks for receiving video and audio data
        /*mediaStreamer.SetVideoHandler([](uint32_t sourceId, const uint8_t* data, size_t len){
            std::cout << "Received video: Length=" << len << " from source #" << sourceId << std::endl;
        });*/
        mediaStreamer.SetAudioHandler([](uint32_t sourceId, const uint8_t* data, size_t len){
            std::cout << "Received audio: Length=" << len << " from source #" << sourceId << std::endl;
        });

        // Initialize the client connection with server
        client.SetServer(client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011)));
        client.JoinChat(client.GetServer());
        client.JoinVideoChat(client.GetServer());

        client.HandleRequestsAsync();
        client.StartStreaming(mediaStreamer);

        //    // Send some random audio/video data for testing
        //    int i=0;
        //    while(true)
        //    {
        //        boost::this_thread::sleep(boost::posix_time::milliseconds(200));
        //        if (i<10)
        //        {
        //            std::vector<char> data;
        //            data.resize(50000);

        //            for (size_t i=0; i<data.size(); ++i)
        //                data[i] = i%255;
        //            if (rand()%2 == 0)
        //                mediaStreamer.Send(MEDIA_VIDEO, (uint8_t*)&data[0], data.size());
        //            else
        //                mediaStreamer.Send(MEDIA_AUDIO, (uint8_t*)&data[0], data.size());

        //            std::cout << "Sending: " << data.size() << std::endl;
        //            i++;
        //        }
        //    }

        //    return 0;

        av_register_all();
        avdevice_register_all();
        GtkWidget *mainWindow;
        gtk_init(&argc, &argv);
        mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_default_size(GTK_WINDOW(mainWindow), 1000, 600);
        gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);
        gtk_widget_set_size_request(mainWindow, 1000, 600);
        g_signal_connect_swapped(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
        GtkCssProvider *provider;
        GdkDisplay *display;
        GdkScreen *screen;
        provider = gtk_css_provider_new();
        display = gdk_display_get_default();
        screen = gdk_display_get_default_screen(display);
        gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        gsize bytes_written, bytes_read;
        const gchar* home = "default.css";
        GError *error = 0;
        gtk_css_provider_load_from_path(provider, g_filename_to_utf8(home, strlen(home), &bytes_read, &bytes_written, &error), NULL);
        g_object_unref(provider);
        GtkWidget* fixed = gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(mainWindow), fixed);
        g_timeout_add(100, TimeoutCallback, mainWindow);


        VideoPlaybackManager vpm;
        vpm.Set(fixed);
        VideoCapture cap;
        cap.StartRecording();
        bool done = false;
        boost::thread streamerThread([&done, &mediaStreamer, &cap]()
        {
            while (!done)
            {
                AVPacket* pkt = cap.GetPacket();
                mediaStreamer.Send(MEDIA_VIDEO, pkt->data, pkt->size);
            }
        });

        mediaStreamer.SetVideoHandler([&vpm](uint32_t sourceId, const uint8_t* data, size_t len){
            //std::cout << "Received video: Length=" << len << " from source #" << sourceId << std::endl;
            vpm.SetPacket(static_cast<int>(sourceId), data, len);
        });
        
        gtk_widget_show_all(mainWindow);
        gtk_main();

        done = true;
        streamerThread.join();
        vpm.Stop();

        cap.StopRecording();

        return 0;
    }
    catch (std::exception& err)
    {
        std::cout << "\n:/ \n" << err.what() << std::endl;
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

