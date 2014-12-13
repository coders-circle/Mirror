#include "client/UI/TextEdit.h"

TextEdit::TextEdit()
{
    m_handle = gtk_entry_new();
    m_type = Control::TEXTEDIT;
    //gtk_button_set
    //gtk_widget_set_size_request
}

void TextEdit::Set( GtkWidget* fixed, int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    //gtk_button_set_label(GTK_BUTTON(m_handle), m_label.c_str());
    gtk_widget_set_size_request(m_handle, w, h);
    this->PutFixedAt(fixed, x, y);
}