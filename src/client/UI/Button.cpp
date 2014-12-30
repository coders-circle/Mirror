#include <common/common.h>
#include "client/UI/Button.h"

Button::Button()
{
    m_handle = gtk_button_new();
    m_type = CONTROL::BUTTON;
}

void Button::Set(std::string label, GtkWidget* fixed, int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_label = label;
    gtk_button_set_label(GTK_BUTTON(m_handle), m_label.c_str());
    gtk_widget_set_size_request(m_handle, w, h);
    this->PutFixedAt(fixed, x, y);
}
