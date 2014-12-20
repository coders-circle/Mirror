#pragma once

#include "client/UI/Page.h"
#include "client/UI/Menu.h"

#include "client/UI/pages/LoginPage.h"


class UIManager
{
public:
    enum MENU { CONNECTIONSMENU = 0, TOOLSMENU, HELPMENU};
    enum PAGE { LOGINPAGE = 0, SETTINGSPAGE, ABOUTPAGE };
    void Initialize(GtkWidget* parent, GtkWidget* fixed)
    {
        m_parent = parent;
        m_fixed = fixed;
        m_menubar.Initialize(parent);
        m_menubar.SetEventHandler(G_CALLBACK(MenuItemEventHandler), this);
        m_menubar.AddMenu("Connections");
        m_menubar.AddMenuItem(MENU::CONNECTIONSMENU, "Connect to a server");
        m_menubar.AddMenuItem(MENU::CONNECTIONSMENU, "Disconnect");
        m_menubar.AddMenu("Tools");
        m_menubar.AddMenuItem(MENU::TOOLSMENU, "Preferences");
        m_menubar.AddMenu("Help");
        m_menubar.AddMenuItem(MENU::HELPMENU, "About");
        m_menubar.FixedPut(fixed);
        m_menubar.Show();

        m_pages.resize(1);
        m_pages[0] = new LoginPage(m_parent, m_fixed);
        LoginPage* loginPage = static_cast<LoginPage*>(m_pages[0]);
    }
    void NavigateTo(int page)
    {
        if (m_currentPage)
        {
            //code to hide the page
        }
        m_currentPage = m_pages[page];
        m_currentPage->ShowControls();
    }
    UIManager()
    {
        m_parent = 0;
        m_fixed = 0;
        m_currentPage = 0;
    }
    static void MenuItemEventHandler(GtkWidget* widget, gpointer data)
    {
        MenuBar::MenuEventData *eventData = static_cast<MenuBar::MenuEventData*>(data);
        UIManager* uiManager = static_cast<UIManager*>(eventData->uiManager);
        switch (eventData->menuID)
        {
        case UIManager::CONNECTIONSMENU:
            if (eventData->menuItemID == 0) std::cout << "So, connect to a server eh!";
            else std::cout << "You want to disconnect?? :/";
            break;
        case UIManager::TOOLSMENU:
            std::cout << "What do you prefer? :v";
            break;
        case UIManager::HELPMENU:
            std::cout << "About us? we are Mirrors!";
        }
        std::cout << std::endl;
    }
private:
    GtkWidget* m_parent;
    GtkWidget* m_fixed;
    std::vector<Page*> m_pages;
    MenuBar m_menubar;
    Page* m_currentPage;
};