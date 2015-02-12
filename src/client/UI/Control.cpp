#include <common/common.h>
#include "client/UI/Control.h"

Control::Control() :m_container(0), m_handle(0)
{

}

void Control::PutFixedAt(GtkWidget *fixed, int x, int y)
{
    m_x = x;
    m_y = y;
    gtk_fixed_put(GTK_FIXED(fixed), m_handle, m_x, m_y);
}

GtkWidget* Control::GetHandle()
{
    return m_handle;
}

void Control::SetContainer(GtkWidget* container)
{
    m_container = container;
}

void Control::MoveTo(int x, int y)
{
    m_x = x;
    m_y = y;
    gtk_fixed_move(GTK_FIXED(m_container), m_handle, m_x, m_y);
}

void Control::Resize(int w, int h)
{
    m_w = w;
    m_h = h;
    gtk_widget_set_size_request(m_handle, m_w, m_h);
}

void Control::SetID(int id)
{
    m_id = id;
}

int Control::GetID()
{
    return m_id;
}

int Control::ID()
{
    return m_id;
}

void Control::Show()
{
    gtk_widget_show(m_handle);
}

void Control::Hide()
{
    gtk_widget_hide(m_handle);
}
