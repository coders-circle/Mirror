/* Client - main.cpp */

#include <common/common.h>

//boost test for separate thread
//void Thread()
//{
//		int i=0;
//		while(i<100)
//		{
//      	std::cout << "From Child Thread" << std::endl;
//			i++;
//		}
//}
//int main()
//{
//    boost::thread t(Thread);
//    std::cout << "From Main Thread" << std::endl;
//	 t.join();
//    return 0;
//}

#include <gtk/gtk.h>

#include "client/FrameRenderer.h"

#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#endif


int main(int argc, char *argv[])
{
  
    GtkWidget *window;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_set_size_request(window, 1000, 600);

    FrameRenderer fr;
    fr.Initialize(window, 100, 100, 256, 256);
    unsigned char* frameData = new unsigned char[256 * 256 * 4];
    for (int i = 0; i < 256 * 256; i++)
    {
        frameData[4 * i + 0] = i % 256;
        frameData[4 * i + 1] = 0;
        frameData[4 * i + 2] = 0;
        frameData[4 * i + 3] = 255;
    }
    fr.SetRGBData(frameData);
    delete frameData;
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
