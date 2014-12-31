#pragma once

#include "client/UI/Control.h"

class Spinner : public Control
{
public:
    Spinner()
    {
        m_handle = gtk_spinner_new();
        m_type = CONTROL::SPINNER;
    }
    void Set(GtkWidget* fixed, int x, int y, int w = 100, int h = 100)
    {
        gtk_widget_set_size_request(m_handle, w, h);
        this->PutFixedAt(fixed, x, y);
    }
    void Start()
    {
        gtk_spinner_start(GTK_SPINNER(m_handle));
    }
    void Stop()
    {
        gtk_spinner_stop(GTK_SPINNER(m_handle));
    }
};