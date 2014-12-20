#pragma once

#include "Control.h"

class TextEdit : public Control
{
public:
    TextEdit();
    //Button(std::string label, int x, int y, int w, int h);
    void Set(GtkWidget* fixed, int x, int y, int w, int h);
    void SetPasswordMode(bool isPassword = true);
    const gchar* GetText();
private:
};