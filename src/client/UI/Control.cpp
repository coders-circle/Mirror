#include "client/UI/Control.h"

void Control::PutFixedAt(GtkWidget *fixed, int x, int y)
{
    m_x = x;
    m_y = y;
    gtk_fixed_put(GTK_FIXED(fixed), m_handle, m_x, m_y);
}

void Control::PutFixedRelativeTo(Control* obj, GtkWidget* fixed, int xOffset, int yOffset)
{
    this->PutFixedAt(fixed, obj->m_x + xOffset, obj->m_y + yOffset);
}

void Control::Show()
{
    gtk_widget_show(m_handle);
}

void Control::Hide()
{
    gtk_widget_hide(m_handle);
}