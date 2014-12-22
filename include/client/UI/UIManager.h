#pragma once

#include "client/UI/Page.h"
#include "client/UI/Menu.h"

#include "client/UI/pages/LoginPage.h"

// UIManager: a custom class to handle pages and menu,
// only one object of the class should be created,
// addition and removal of pages and menus can be 
// done in UIManager::Initialize() function
class UIManager
{
public:
    enum MENU { CONNECTIONSMENU = 0, TOOLSMENU, HELPMENU};
    enum PAGE { LOGINPAGE = 0, SETTINGSPAGE, ABOUTPAGE };

    // Creates and initializes the menus and pages used
    // in the application
    void Initialize(GtkWidget* parent, GtkWidget* fixed);

    // Makes transition between pages
    // 'page' should be one of the value in enum 'PAGE'
    void NavigateTo(int page);
    
    UIManager();

    // called when any menu item is pressed
    // data is a pointer of MenuBar::MenuEventData type
    static void MenuItemEventHandler(GtkWidget* widget, gpointer data);
private:
    GtkWidget* m_parent;
    GtkWidget* m_fixed;
    std::vector<Page*> m_pages;
    MenuBar m_menubar;
    Page* m_currentPage;
};