/* Client - main.cpp */

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <malloc.h>
//#include <crtdbg.h>

#include <common/common.h>
#include "client/Application.h"
#include "client/MediaStream/VideoCapture.h"
#include "client/MediaStream/AudioStream.h"
#include "client/MediaStream/VideoPlayback.h"



VideoPlayback *videoPlayback;

gboolean IdleFunction(gpointer userData)
{
    return FALSE;
}

int main(int argc, char *argv[]) try
{
    /*_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetBreakAlloc(18);*/

    // libav Audio/Video Initialization
    //---------------------------------
    av_register_all();
    avdevice_register_all();
    videoPlayback = new VideoPlayback();
    videoPlayback->Test();

    // GTK+ initialization
    //--------------------
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
    videoPlayback->Set(fixed, 10, 10);
    videoPlayback->StartPlaybackAsync();
    gtk_widget_show_all(mainWindow);
    
    /*Application app;
    app.Initialize(mainWindow, fixed);*/
    
    gtk_main();
    return 0;
}
catch (std::exception err)
{
    std::cout << ":/ \n" << err.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cout << "\n:/ \n" << "Unexpected exception occured!" << std::endl;
    return 1;
}


#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "gdk-win32-3.0.lib")

//#pragma comment(lib, "opencv_highgui245.lib")
//#pragma comment(lib, "opencv_core245.lib")
//#pragma comment(lib, "opencv_imgproc245.lib")
//#pragma comment(lib, "opencv_video245.lib")
#endif

