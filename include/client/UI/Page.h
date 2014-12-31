#pragma once

#include "common/Exception.h"

#include "Control.h"
#include "Button.h"
#include "Label.h"
#include "TextEdit.h"
#include "Spinner.h"

#include <iostream>
#include <vector>
#include <stdio.h>

class Page;

// A struct to wrap the data passed to event handler to the pages
struct ControlEventData
{
    Page* page;         
    Control* control;
    int eventID;
    ControlEventData(Page* page, Control* control, int event_id) :page(page), control(control), eventID(eventID){}
};

typedef void(*PageEventCallBack)(ControlEventData*);
enum CONTROLEVENT{CLICKED};


// a general class to group controls and create a page
// acts as base class for various pages
class Page
{
public:

    // Don't use:  Under Construction!
    // Loads the page from a file
    void LoadFromFile(std::string fileName);

    // Adds a button with specified label in the specified coordinate,
    // id = unique identifier for the control
    // returns a pointer to the created object
    // typecast to Button*
    Control* AddButton(int id, std::string label, int x, int y, int w, int h);

    // Adds a text label with specified text in the specified coordinate,
    // the text may contain pango markup syntax,
    // returns a pointer to the created object
    // typecast to Label*
    Control* AddLabel(std::string text, int x, int y, int w = 0, int h = 0, int justification = GTK_JUSTIFY_CENTER);

    // Adds a text label with specified text in the specified coordinate,
    // id = unique identifier for the control
    // the text may contain pango markup syntax,
    // returns a pointer to the created object
    // typecast to Label*
    Control* AddLabel(int id, int x, int y, int w, int h);

    // Adds a Edit Box in the specified coordinate,
    // id = unique identifier for the control
    // returns a pointer to the created object
    // typecast to TextEdit*
    Control* AddTextEdit(int id, int x, int y, int w, int h);

    // Adds an animated Spinner int the specified coordinate
    // returns a pointer to the created object
    // typecast to Spinner*
    Control* AddSpinner(int x, int y, int w = 20, int h = 20);

    // returns width of parent window
    int GetParentWidth();

    // returns height of parent window
    int GetParentHeight();

    // shows all controls within the page
    virtual void ShowControls();

    // hide all controls within the page
    virtual void HideControls();

    // called when an event by controls within occurs 
    // to be implemented by derived class
    virtual void OnControlEvent(Control*, int eventID);

    // initializes the page
    // must be called before adding controls
    void Initialize(GtkWidget* parentWindow, GtkWidget* fixed);

    //returns a control within the page specified by the id
    Control* GetControlByID(unsigned int controlID);

    // Handles the events for the controls in the page
    static void ControlEventHandler(GtkWidget* widget, gpointer data);

    // set a event handler procedure which will be called
    // whenever a control within the page trigers any event.
    // a pointer to ControlEventData object will be passed along
    // for the detail of the event
    void SetEventHandler(PageEventCallBack eventHandler);



    // returns the page id
    // page id uniquely determines the page
    int GetID();
    
protected:
    // Allocates memory for a specified new control to be added,
    // also sets a unique id to that control
    // to be internally used by the Page class
    // TODO: maybe move the function to private section
    void AllocateNewControl(int type, int id);

    // Array of controls present in the page
    std::vector<Control*> m_controls;

    // container for the controls (fixed type)
    GtkWidget* m_fixed;

    // Handle to parent window
    GtkWidget* m_parentWindow;

    // event handler of the page,
    // is called whenever specific events are triggered within the page
    PageEventCallBack m_pageEventHandler;
    
    // unique id for the page
    int m_ID;
    
};



