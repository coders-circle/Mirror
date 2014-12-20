#include "client/UI/TextEdit.h"

TextEdit::TextEdit()
{
    m_handle = gtk_entry_new();
    m_type = Control::TEXTEDIT;
}

void TextEdit::Set( GtkWidget* fixed, int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    gtk_widget_set_size_request(m_handle, w, h);
    this->PutFixedAt(fixed, x, y);
}

void TextEdit::SetPasswordMode(bool isPassword)
{
    gtk_entry_set_visibility(GTK_ENTRY(m_handle), (!isPassword));
}

const gchar* TextEdit::GetText()
{
    return gtk_entry_get_text(GTK_ENTRY(m_handle));
}