#pragma once

#include "Control.h"

#include <iostream>

class Label : public Control
{
public:
    std::string buff;
    Label();
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w = 0, int h = 0);
    void Set(int id, GtkWidget* fixed, int x, int y, int w, int h)
    {
        this->m_id = id;
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;

       
        scw = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scw), m_w);
        gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scw), m_h);
        
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scw ), m_handle);
        
        gtk_fixed_put(GTK_FIXED(fixed), scw, m_x, m_y);
        gtk_widget_set_halign(m_handle, GTK_ALIGN_START);
        gtk_widget_set_valign(m_handle, GTK_ALIGN_END);
    }
    void ScrollToEnd()
    {
        GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scw));
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
    }
   
    void Show()
    {
        if (scw)
            gtk_widget_show(scw);
        
        Control::Show();
    }
    void Hide()
    {
        if (scw)
            gtk_widget_hide(scw);
        Control::Hide();
    }
    void AppendToNewLine(std::string text)
    {
        this->ScrollToEnd();
        buff += std::string("\n");
        buff += text;
        std::string temp = buff + std::string("\n");
        gtk_label_set_label(GTK_LABEL(m_handle), temp.c_str());
        this->ScrollToEnd();
    }
    void SetJustify(int justification)
    {
        //gtk_label_set_justify(GTK_LABEL(this->m_handle), (GtkJustification)justification);
        //gtk_widget_set_halign(this->m_handle, GTK_ALIGN_START);
    }
private:
    GtkWidget* scw;
};
