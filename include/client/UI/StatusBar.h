#pragma once

#include "client/UI/Control.h"

class Statusbar: public Control
{
public:
    Statusbar();
    void Set(GtkWidget* fixed);
private:
};