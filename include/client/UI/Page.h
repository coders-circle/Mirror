#pragma once

#include "common/Exception.h"

#include "Control.h"
#include "Button.h"
#include "Label.h"
#include "TextEdit.h"

#include <iostream>
#include <vector>
#include <stdio.h>

class Page;


struct ControlEventData
{
    Page* page;
    Control* control;
    int eventID;
    ControlEventData(Page* page, Control* control, int event_id) :page(page), control(control), eventID(eventID){}
};

typedef void(*PageEventCallBack)(ControlEventData*);

enum CONTROLEVENT{CLICKED};



class Page
{
public:

    // Don't use:  Under Construction!
    // Loads the page from a file
    void LoadFromFile(std::string fileName);

    // Adds a button with specified label in the specified coordinate,
    // returns a unique ID for the control in the page
    Control* AddButton(int id, std::string label, int x, int y, int w, int h);

    // Adds a text label with specified text in the specified coordinate,
    // the text can contain certain markups,
    // returns a unique ID for the control in the page
    Control* AddLabel(std::string text, int x, int y, int w = 0, int h = 0, int justification = GTK_JUSTIFY_CENTER);

    // Adds a Edit Box in the specified coordinate,
    // returns a unique ID for the control in the page
    Control* AddTextEdit(int id, int x, int y, int w, int h);

    // shows all controls within the page
    void ShowControls();

    // hide all controls within the page
    void HideControls();

    // called when an event by controls within occurs 
    // to be implemented by derived class
    virtual void OnControlEvent(Control*, int eventID);

    void Initialize(GtkWidget* parentWindow, GtkWidget* fixed);

    //return a control specified by the id
    Control* GetControlByID(unsigned int controlID);

    // Handles the events for the controls in the page
    static void ControlEventHandler(GtkWidget* widget, gpointer data);

    void SetEventHandler(PageEventCallBack eventHandler);

    int GetID();

    

    
protected:
    // Allocates memory for a specified new control to be added,
    // also sets a unique id to that control starting from 0
    void AllocateNewControl(int type, int id);

    // Array of controls present in the page
    std::vector<Control*> m_controls;

    GtkWidget* m_fixed;
    GtkWidget* m_parentWindow;

    PageEventCallBack m_pageEventHandler;
    // A struct to wrap the data passed to event handler to the pages

    int m_ID;
    
};

enum PAGE { LOGINPAGE = 0, SIGNUPPAGE, ABOUTPAGE, SVCONNECTPAGE};

