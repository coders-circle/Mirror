#include "client/UI/FrameRenderer.h"

#include <cstring>

FrameRenderer::FrameRenderer()
{
    m_drawingArea = 0;
    m_rgbData = 0;
    m_rgbImage = 0;
}

void FrameRenderer::Show()
{
    gtk_widget_show(m_drawingArea);
}
void FrameRenderer::SetRGBData(unsigned char* rgbData)
{
    for (int i1 = 0, i2 = 0, lim = m_w*m_h * 4; i1 < lim; i1 += 4, i2+=3)
    {
        m_rgbData[i1 + 0] = rgbData[i2 + 2];
        m_rgbData[i1 + 1] = rgbData[i2 + 1];
        m_rgbData[i1 + 2] = rgbData[i2 + 0];
        m_rgbData[i1 + 3] = 255;
    }
    gtk_widget_queue_draw_area(m_drawingArea, 0, 0, m_w, m_h);
}

gboolean FrameRenderer::OnDraw(GtkWidget* widget, cairo_t* cr, gpointer frPointer)
{
    FrameRenderer* fr = (FrameRenderer*)frPointer;
    cairo_set_source_surface(cr, fr->m_rgbImage, 0, 0);
    cairo_paint(cr);
    return FALSE;
}

void FrameRenderer::Set(GtkWidget* fixed, int x, int y, int fw, int fh)
{
    m_x = x;
    m_y = y;
    m_w = fw;
    m_h = fh;
    
    if (m_drawingArea)  delete m_drawingArea;
    if (m_rgbData)      delete m_rgbData;
    if (m_rgbImage)     cairo_surface_destroy(m_rgbImage);

    
    m_rgbData       = new unsigned char[4*m_w*m_h];
    m_drawingArea   = gtk_drawing_area_new();
    m_rgbImage = cairo_image_surface_create_for_data(m_rgbData, CAIRO_FORMAT_ARGB32, m_w, m_h, m_w*4);

    gtk_widget_set_size_request(m_drawingArea, fw, fh);
    g_signal_connect(G_OBJECT(m_drawingArea), "draw", G_CALLBACK(this->OnDraw), this);
    gtk_fixed_put(GTK_FIXED(fixed), m_drawingArea, m_x, m_y);
}