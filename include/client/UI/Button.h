#ifndef __BUTTON__
#define __BUTTON__

#include "Control.h"

class Button : public Control
{
public:
    Button();
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w, int h);
private:
};

#endif