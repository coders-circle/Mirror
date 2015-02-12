#include "client/UI/FrameRenderer.h"

#include <cstring>

FrameRenderer::FrameRenderer()
{
    m_handle = 0;
    m_rawBGRAData = 0;
    m_drawingSurface = 0;
}

void FrameRenderer::Show()
{
    gtk_widget_show(m_handle);
}
void FrameRenderer::SetRGBData(unsigned char* rgbData)
{
    for (int i1 = 0, i2 = 0, lim = m_w*m_h * 4; i1 < lim; i1 += 4, i2+=3)
    {
        m_rawBGRAData[i1 + 0] = rgbData[i2 + 2];
        m_rawBGRAData[i1 + 1] = rgbData[i2 + 1];
        m_rawBGRAData[i1 + 2] = rgbData[i2 + 0];
        m_rawBGRAData[i1 + 3] = 255;
    }
    gtk_widget_queue_draw_area(m_handle, 0, 0, m_w, m_h);
}
void FrameRenderer::SetBGRAData(unsigned char* bgraData)
{
    memcpy(m_rawBGRAData, bgraData, (m_w*m_h) << 2);
    gtk_widget_queue_draw(m_handle);
    //gtk_widget_queue_draw_area(m_)
}


gboolean FrameRenderer::OnDraw(GtkWidget* widget, cairo_t* cr, gpointer frPointer)
{
    FrameRenderer* fr = (FrameRenderer*)frPointer;
    cairo_set_source_surface(cr, fr->m_drawingSurface, 0, 0);
    cairo_paint(cr);
    return FALSE;
}


void FrameRenderer::Set(GtkWidget* fixed, int x, int y, int fw, int fh)
{
    m_x = x;
    m_y = y;
    m_w = fw;
    m_h = fh;
    
    if (m_handle)           delete m_handle;
    if (m_rawBGRAData)      delete m_rawBGRAData;
    if (m_drawingSurface)   cairo_surface_destroy(m_drawingSurface);

    
    m_rawBGRAData       = new unsigned char[4*m_w*m_h];
    m_handle   = gtk_drawing_area_new();
    m_drawingSurface = cairo_image_surface_create_for_data(m_rawBGRAData, CAIRO_FORMAT_ARGB32, m_w, m_h, m_w*4);

    gtk_widget_set_size_request(m_handle, fw, fh);
    g_signal_connect(G_OBJECT(m_handle), "draw", G_CALLBACK(this->OnDraw), this);
    gtk_fixed_put(GTK_FIXED(fixed), m_handle, m_x, m_y);
}