#ifndef __PAGE__
#define __PAGE__

#include "Control.h"

#include <iostream>
#include <vector>

class Page
{
public:
protected:
    std::vector<Control*> m_controls;
};

#endif