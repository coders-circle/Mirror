#include "client/FrameRenderer.h"

#include <cstring>

void FrameRenderer::SetRGBData(unsigned char* rgbData)
{
    memcpy(m_rgbData, rgbData, m_fw*m_fh*4);
}

gboolean FrameRenderer::OnDraw(GtkWidget* widget, cairo_t* cr, gpointer frPointer)
{
    FrameRenderer* fr = (FrameRenderer*)frPointer;
    cairo_set_source_surface(cr, fr->m_rgbImage, 0, 0);
    cairo_paint(cr);
    return FALSE;
}

void FrameRenderer::Initialize(GtkWidget* parentWindow, int x, int y, int fw, int fh)
{
    m_x = x;
    m_y = y;
    m_fw = fw;
    m_fh = fh;
    m_parentWindow = parentWindow;
    
    if (m_drawingArea)  delete m_drawingArea;
    if (m_rgbData)      delete m_rgbData;
    if (m_rgbImage)     cairo_surface_destroy(m_rgbImage);

    
    m_rgbData       = new unsigned char[4*m_fw*m_fh];
    m_drawingArea   = gtk_drawing_area_new();
    m_rgbImage = cairo_image_surface_create_for_data(m_rgbData, CAIRO_FORMAT_ARGB32, m_fw, m_fh, m_fw*4);
    m_fixed = gtk_fixed_new();
    gtk_widget_set_size_request(m_drawingArea, fw, fh);
    gtk_container_add(GTK_CONTAINER(m_parentWindow), m_fixed);
    g_signal_connect(G_OBJECT(m_drawingArea), "draw", G_CALLBACK(this->OnDraw), this);
    gtk_fixed_put(GTK_FIXED(m_fixed), m_drawingArea, m_x, m_y);
}