#pragma once

#include "Control.h"

class Label : public Control
{
public:
    std::string buff;
    Label();
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w = 0, int h = 0);
    void Set(int id, GtkWidget* fixed, int x, int y)
    {
        this->m_id = id;
        m_x = x;
        m_y = y;
        m_w = 0;
        m_h = 0;

        // enables the label to contain some markup syntax

        gtk_widget_set_size_request(m_handle, m_w, m_h);
        this->PutFixedAt(fixed, x, y);
    }
    void AppendToNewLine(std::string text)
    {
        buff += std::string("\n");
        buff += text;
        gtk_label_set_markup(GTK_LABEL(m_handle), buff.c_str());
    }
    void SetJustify(int justification)
    {
        //gtk_label_set_justify(GTK_LABEL(this->m_handle), (GtkJustification)justification);
        //gtk_widget_set_halign(this->m_handle, GTK_ALIGN_START);
    }
private:
};
