#ifndef __BUTTON__
#define __BUTTON__

#include "Control.h"

class Button : public Control
{
public:
    Button();
    //Button(std::string label, int x, int y, int w, int h);
    void Set(std::string label, GtkWidget* fixed, int x, int y, int w, int h);
    //void SetOnClick()
private:
};

#endif