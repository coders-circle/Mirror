#include <common/common.h>
#include "client/UI/StatusBar.h"

Statusbar::Statusbar()
{
    m_handle = gtk_statusbar_new();
    m_type = CONTROL::STATUSBAR;
}

void Statusbar::Set(GtkWidget* fixed)
{
    gtk_widget_set_size_request(m_handle, 200, 30);
    this->PutFixedAt(fixed, 0, 0);
    
}
