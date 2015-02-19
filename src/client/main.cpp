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



//VideoPlayback *v;

//Client client;
int main(int argc, char *argv[])
try
{
    av_register_all();
    avdevice_register_all();

    /*std::string ip;
    std::cout << "Enter server ip: ";
    std::cin >> ip;
    

    client.SetServer(client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011)));
    client.JoinChat(client.GetServer());
    client.JoinVideoChat(client.GetServer());
    
  
    

    v = new VideoPlayback();*/
    //v->InitializeDecoder();
    
    //client.StartReceivingAV(v, &cap);

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

    
    g_timeout_add(100, TimeoutCallback, mainWindow);
        //v->Set(fixed, 10, 10);
    //v->StartPlaybackAsync();

    /*VideoCapture cap;
    cap.StartRecording();*/

    VideoPlaybackManager vpm;
    RandomVideoGenerator rv0, rv1, rv2, rv3;
    vpm.Set(fixed);
    volatile bool done = false;
    
    boost::thread testThread0 = boost::thread([&done, &vpm, &rv0]()
    {
        while (!done){
            vpm.SetPacket(0, rv0.GetPacket());
        }
    });
    boost::thread testThread1 = boost::thread([&done, &vpm, &rv1]()
    {
        while (!done){
            vpm.SetPacket(1, rv1.GetPacket());
        }
    });
    boost::thread testThread2 = boost::thread([&done, &vpm, &rv2]()
    {
        while (!done){
            vpm.SetPacket(2, rv2.GetPacket());
        }
    });
    boost::thread testThread3 = boost::thread([&done, &vpm, &rv3]()
    {
        while (!done){
            vpm.SetPacket(3, rv3.GetPacket());
        }
    });

    gtk_widget_show_all(mainWindow);
    /*Application app;
    app.Initialize(mainWindow, fixed);*/
    gtk_main();
    done = true;
    //cap.StopRecording();
    testThread1.join();
    testThread2.join();
    testThread3.join();
    testThread0.join();
    rv0.End();
    rv1.End();
    rv2.End();
    rv3.End();
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


#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "gdk-win32-3.0.lib")
#endif

