/* Client - main.cpp */



#include <common/common.h>
#include "client/FrameRenderer.h"
#include "client/VideoCapture.h"
#include "client/UI/Page.h"



FrameRenderer fr;
VideoCapture vidCap;


gboolean IdleFunction(gpointer userData)
{
    //fr.SetRGBData(vidCap.GetBGRAFrame());
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_set_size_request(window, 1000, 600);
    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //g_idle_add(IdleFunction, 0);
    //vidCap.Initialize();
    GtkWidget* fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);
    fr.Initialize(window, fixed, 10, 10, vidCap.GetFrameWidth(), vidCap.GetFrameHeight());
    gtk_widget_show_all(window);
    Page p;
    
    
    p.Initialize(window, fixed);
    p.AddLabel("<b>Username:</b>", 300, 100, 100, 20);
    p.AddLabel("<b>Password:</b> ", 300, 200, 100, 20);
    p.AddTextEdit(410, 100, 200, 25);
    p.AddTextEdit(410, 200, 200, 25);
    p.AddButton("Sign In", 400, 300, 100, 30);
    p.ShowControls();
    gtk_main();
    
    
    return 0;
}



#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "opencv_highgui245.lib")
#pragma comment(lib, "opencv_core245.lib")
#pragma comment(lib, "opencv_imgproc245.lib")
#endif

