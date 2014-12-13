#ifndef __PAGE__
#define __PAGE__

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "Control.h"
#include "Button.h"
#include "Label.h"
#include "TextEdit.h"

#include <iostream>
#include <vector>
#include <stdio.h>

class Page
{
public:
    void LoadFromFile(std::string fileName);
    void AddButton(std::string label, int x, int y, int w, int h);
    void AddLabel(std::string label, int x, int y, int w, int h);
    void AddTextEdit(int x, int y, int w, int h);
    void ShowControls();
    void Initialize(GtkWidget* parentWindow, GtkWidget* fixed);
protected:
    std::vector<Control*> m_controls;
    GtkWidget* m_fixed;
    GtkWidget* m_parentWindow;
};

#endif