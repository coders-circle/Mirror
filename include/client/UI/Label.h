#pragma once

#include "Control.h"

class Label : public Control
{
public:
    Label();
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w = 0, int h = 0);

    // ----------------------------------------------------------------------
    // Sets the text justification for the label
    // 'justification' can be one of the following value
    // GTK_JUSTIFY_LEFT
    // GTK_JUSTIFY_RIGHT
    // GTK_JUSTIFY_CENTER
    // GTK_JUSTIFY_FILL - The text is placed is distributed across the label.
    // ----------------------------------------------------------------------
    void SetJustify(int justification)
    {
        //gtk_label_set_justify(GTK_LABEL(this->m_handle), (GtkJustification)justification);
        //gtk_widget_set_halign(this->m_handle, GTK_ALIGN_START);
    }
private:
};
