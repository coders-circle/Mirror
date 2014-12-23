/* Client - main.cpp */



#include <common/common.h>
#include "client/FrameRenderer.h"
#include "client/VideoCapture.h"
#include "client/UI/Page.h"

#include "client/UI/pages/LoginPage.h"
#include "client/UI/Menu.h"
#include "client/UI/UIManager.h"



//FrameRenderer fr;
//VideoCapture vidCap;


gboolean IdleFunction(gpointer userData)
{
    //fr.SetRGBData(vidCap.GetBGRAFrame());
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkWidget *mainWindow;
    gtk_init(&argc, &argv);
    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    gtk_window_set_default_size(GTK_WINDOW(mainWindow), 1000, 600);
    gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);
    gtk_widget_set_size_request(mainWindow, 1000, 600);
    g_signal_connect_swapped(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget* fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(mainWindow), fixed);

    gtk_widget_show_all(mainWindow);

    UIManager uiManager;
    uiManager.Initialize(mainWindow, fixed);
    uiManager.NavigateTo(PAGE::LOGINPAGE);

    
    
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

