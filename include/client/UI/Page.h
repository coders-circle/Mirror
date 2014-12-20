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

    // Don't use:  Under Construction!
    // Loads the page from a file
    void LoadFromFile(std::string fileName);

    // Adds a button with specified label in the specified coordinate,
    // returns a unique ID for the control in the page
    int AddButton(std::string label, int x, int y, int w, int h);

    // Adds a text label with specified text in the specified coordinate,
    // the text can contain certain markups,
    // returns a unique ID for the control in the page
    int AddLabel(std::string text, int x, int y, int w, int h);

    // Adds a Edit Box in the specified coordinate,
    // returns a unique ID for the control in the page
    int AddTextEdit(int x, int y, int w, int h);

    // shows all controls within the page
    void ShowControls();

    // called when an event by controls within occurs 
    // to be implemented by derived class
    virtual void OnControlEvent(int control_id);

    void Initialize(GtkWidget* parentWindow, GtkWidget* fixed);

    // TODO: move the function implementation to .cpp
    // TODO: add proper exception
    Control* GetControlByID(unsigned int control_id)
    {
        if (control_id >= 0 && control_id < m_controls.size())
        {
            return m_controls[control_id];
        }
        throw "invalid control ID";
    }

    // Handles the events for the controls in the page
    static void ControlEventHandler(GtkWidget* widget, gpointer data);
protected:
    // Allocates memory for a specified new control to be added,
    // also sets a unique id to that control starting from 0
    void AllocateNewControl(Control::ControlType type);

    // Array of controls present in the page
    std::vector<Control*> m_controls;

    GtkWidget* m_fixed;
    GtkWidget* m_parentWindow;

    enum ControlEvent{BUTTON_CLICK = 0};

    struct EventData
    {
        Page* page;
        Control* control;
        int event_id;
        EventData(Page* page, Control* control, int event_id) :page(page), control(control), event_id(event_id){}
    };
};

#endif