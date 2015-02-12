#pragma once

#include "client/UI/Control.h"

class Button : public Control
{
public:
    Button();
    Button(std::string label, GtkWidget* fixed, int x, int y, int w = 0, int h = 0);
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w = 0, int h = 0);
private:
};

