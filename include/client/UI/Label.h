#pragma once

#include "Control.h"

class Label : public Control
{
public:
    Label();
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w = 0, int h = 0);

    void SetJustify(int justification)
    {
        //gtk_label_set_justify(GTK_LABEL(this->m_handle), (GtkJustification)justification);
        //gtk_widget_set_halign(this->m_handle, GTK_ALIGN_START);
    }
private:
};
