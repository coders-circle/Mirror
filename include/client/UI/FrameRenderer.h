#pragma once

#include "common/common.h"
#include "client/UI/Control.h"
#include "cairo.h"

class FrameRenderer:public Control
{
public:
    FrameRenderer();
    void SetRGBData(unsigned char* rgbData);
    void Set(GtkWidget* fixed, int x, int y, int fw, int fh);
    static gboolean OnDraw(GtkWidget* widget, cairo_t* cr, gpointer frPointer);
    void Show();
    void SetBGRAData(unsigned char* bgraData);
    void Resize(int w, int h);
private:
    cairo_surface_t *m_drawingSurface;
    std::vector<unsigned char> m_rawBGRAData;
};
