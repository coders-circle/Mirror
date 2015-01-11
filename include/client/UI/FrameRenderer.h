#pragma once

#include "common/common.h"
#include "client/UI/Control.h"
#include "cairo.h"

class FrameRenderer:public Control
{
public:
    FrameRenderer();
    void SetRGBData(unsigned char* rgbData);
    void SetBGRAData(unsigned char* bgraData);
    void Set(GtkWidget* fixed, int x, int y, int fw, int fh);
    static gboolean OnDraw(GtkWidget* widget, cairo_t* cr, gpointer frPointer);
    void Show();
private:
    GtkWidget *m_drawingArea;
    cairo_surface_t *m_surface;
    unsigned char* m_bgraData;
};
