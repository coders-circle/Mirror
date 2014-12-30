#include "client/UI/Label.h"

Label::Label()
{
    m_handle = gtk_label_new(NULL);
    m_type = CONTROL::LABEL;
    scw = 0;
}

void Label::Set(std::string label, GtkWidget* fixed, int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_label = label;

    // enables the label to contain some markup syntax
    gtk_label_set_markup(GTK_LABEL(m_handle), m_label.c_str());

    gtk_widget_set_size_request(m_handle, w, h);
    this->PutFixedAt(fixed, x, y);
}