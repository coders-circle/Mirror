/* Client - main.cpp */

#include <common/common.h>
#include "client/Application.h"
#include "client/MediaStream/VideoCapture.h"
#include "client/MediaStream/AudioStream.h"
#include "client/MediaStream/VideoPlayback.h"


VideoPlayback *v;

Client client;
RtpStreamer rtps;
int main(int argc, char *argv[])
try
{
    std::string ip;
    std::cout << "Enter server ip: ";
    std::cin >> ip;
    av_register_all();
    avdevice_register_all();

    client.SetServer(client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011)));
    client.JoinChat(client.GetServer());
    client.JoinVideoChat(client.GetServer());

    rtps.Initialize(&client.GetUdpHandler1());
    boost::thread rtpsre ([](){
        rtps.StartReceiving();
    });
   
    VideoCapture cap;
    cap.StartRecording();

    v = new VideoPlayback();
    //v->InitializeDecoder();
    
    GtkWidget *mainWindow;
    gtk_init(&argc, &argv);
    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(mainWindow), 1000, 600);
    gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);
    gtk_widget_set_size_request(mainWindow, 1000, 600);
    g_signal_connect_swapped(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //g_idle_add()
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

    v->Set(fixed, 10, 10);
    v->StartPlaybackAsync();

    VideoCapture* capp = &cap;
    boost::thread sendthread([capp](){
        while(1)
        {
            capp->SendRtp(rtps, client.GetUdpEndpoint(client.GetServer()));
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            v->ReceiveRtp(rtps);
        }
    });
    gtk_widget_show_all(mainWindow);
    /*Application app;
    app.Initialize(mainWindow, fixed);*/
    gtk_main();
    rtps.StopReceiving();
    cap.StopRecording();
    return 0;
}
catch (std::exception err)
{
    std::cout << "\n:/ \n" << err.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cout << "\n:/ \n" << "Unexpected exception occured"  << std::endl;
    return 1;
}


#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "gdk-win32-3.0.lib")
#endif

