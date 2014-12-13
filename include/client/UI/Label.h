#pragma once

#include "Control.h"

class Label : public Control
{
public:
    Label();
    //Button(std::string label, int x, int y, int w, int h);
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w, int h);
private:
};
