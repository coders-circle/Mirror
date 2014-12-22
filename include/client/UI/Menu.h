#pragma once

#include "common/Exception.h"

#include <vector>
#include <string>
#include <gtk/gtk.h>

// class to represent individual menu items
class MenuItem
{
public:
    MenuItem() :m_handle(0){}

    // creates a menu item with given label
    void Create(const std::string& label);

    // returns a widget handle representing the item
    GtkWidget* GetHandle();
private:
    GtkWidget* m_handle;
};


// class to represent group of menu items
// i.e a single menu
class Menu
{
public:
    Menu() :m_handle(0), m_menu(0){}

    // creates a menu with specified label
    void Create(const std::string& label);

    // adds  an item to the menu,
    // returns the id for the current item in the menu
    // id is the index of the item starting from zero
    int AddItem(const std::string& label);

    // returns a widget handle representing a specfic item
    // specified by 'index' 
    // index and id are the same
    GtkWidget* GetItemHandle(unsigned int index);

    // return handle to the widget representing the menu list
    GtkWidget* GetHandle();

    // return handle to the widget representing the menu itself
    GtkWidget* GetMenu();
private:
    GtkWidget* m_handle;
    GtkWidget* m_menu;
    std::vector<MenuItem> m_items;
};


// class to represent a menu bar
class MenuBar
{
public:
    struct MenuEventData
    {
        int menuID;
        int menuItemID;
        void* uiManager;
        MenuEventData(int menuID, int menuItemID, void* uiManager) :menuID(menuID),
            menuItemID(menuItemID), uiManager(uiManager){}
    };
    MenuBar() :m_handle(0), m_parent(0), m_menuEventHandler(0){}

    void FixedPut(GtkWidget* fixed);
    void Initialize(GtkWidget* parent);

    // Adds a menu
    void AddMenu(std::string label);

    // Adds an item to a specific menu
    void AddMenuItem(unsigned int menuID, std::string itemLabel);

    // shows all the menu
    void Show();

    // sets the event handler function 
    // and the data to be passed withing the handler
    void SetEventHandler(GCallback eventHandler, void* data);
private:
    GtkWidget* m_handle;
    GtkWidget* m_parent;
    std::vector<Menu> m_menus;
    GCallback m_menuEventHandler;
    void* m_eventData;
};