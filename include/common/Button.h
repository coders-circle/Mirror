#ifndef __BUTTON__
#define __BUTTON__

#include "Control.h"

class Button : public Control
{
public:
    Button();
    Button(std::string label, int x, int y, int w, int h);
private:
};

#endif