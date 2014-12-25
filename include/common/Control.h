#ifndef __CONTROL__
#define __CONTROL__

#include <gtk/gtk.h>
#include <string>

class Control
{
public:
    Control() :m_handle(0){}
    void PutFixedAt(GtkWidget *fixed, int x, int y);
    void PutFixedRelativeTo(GtkWidget* fixed, int xOffset, int yOffset);
protected:
    GtkWidget* m_handle;
    int m_x, m_y, m_w, m_h;
    std::string m_label;
};

#endif