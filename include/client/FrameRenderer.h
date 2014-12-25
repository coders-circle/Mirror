#ifndef __FRAMERENDERER__
#define __FRAMERENDERER__

#include <common/common.h>
#include <cairo.h>

class FrameRenderer
{
public:
    FrameRenderer() :m_parentWindow(0), m_drawingArea(0), m_rgbData(0), m_rgbImage(0){}
    void SetRGBData(unsigned char* rgbData);
    void Initialize(GtkWidget* parentWindow, GtkWidget* fixed, int x, int y, int fw, int fh);
    static gboolean OnDraw(GtkWidget* widget, cairo_t* cr, gpointer frPointer);
private:
    GtkWidget* m_parentWindow;
    GtkWidget* m_fixed;
    GtkWidget *m_drawingArea;
    int m_x, m_y, m_fw, m_fh;
    cairo_surface_t *m_rgbImage;
    unsigned char* m_rgbData;
};









#endif