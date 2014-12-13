#ifndef __CONTROL__
#define __CONTROL__

#include <gtk/gtk.h>
#include <string>

class Control
{
public:
    enum ControlType{ BUTTON, LABEL, TEXTEDIT };
    Control() :m_handle(0){}
    void PutFixedAt(GtkWidget *fixed, int x, int y);
    void PutFixedRelativeTo(Control* obj, GtkWidget* fixed, int xOffset, int yOffset);
    void Show();
    void Hide();
protected:
    GtkWidget* m_handle;
    int m_x, m_y, m_w, m_h;
    std::string m_label;
    ControlType m_type;
    int m_id;
};

#endif