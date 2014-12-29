#pragma once

#include "client/UI/UIManager.h"

// A specific class to handle overall operations

class Application
{
public:
    enum MENU{ CONNECTIONS = 0, TOOLS, HELP };
    enum MENUITEM{ SVCONNECT = 0, DISCONNECT, PREFERENCE, ABOUT };
    enum TOOLITEM{ BACK = 0 };
    Application()
    {
        if (app)
        {
            throw "only 1 instance is allowed";
        }
        app = this;
    }
    static void EventHandler()
    {

    }
    static void UIEventHandler(UIEventData* eventData)
    {
        switch (eventData->event)
        {
        case EVENT::MENUITEMCLICK:
            switch (eventData->controlID)
            {
            case MENUITEM::ABOUT:
                app->uiManager.NavigateTo(PAGE::ABOUTPAGE);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        std::cout << "Some shit just happened";
    }
    void Initialize(GtkWidget* parent, GtkWidget* fixed)
    {
        

        uiManager.Initialize(parent, fixed);
        uiManager.SetEventHandler(UIEventHandler);

        uiManager.AddMenu("Connections", MENU::CONNECTIONS);
        uiManager.AddMenuItem(MENU::CONNECTIONS, "Connect to a server", MENUITEM::SVCONNECT);
        uiManager.AddMenuItem(MENU::CONNECTIONS, "Disconnect", MENUITEM::DISCONNECT);

        uiManager.AddMenu("Tools", MENU::TOOLS);
        uiManager.AddMenuItem(MENU::TOOLS, "Preference", MENUITEM::PREFERENCE);
        
        uiManager.AddMenu("Help", MENU::HELP);
        uiManager.AddMenuItem(MENU::HELP, "About", MENUITEM::ABOUT);

        uiManager.AddPage(new LoginPage(parent, fixed));
        uiManager.AddPage(new AboutPage(parent, fixed));
        uiManager.AddPage(new SVConnectPage(parent, fixed));

        uiManager.AddToolItemFromStock(GTK_STOCK_GO_BACK, TOOLITEM::BACK);

        uiManager.ShowMenu();
        uiManager.ShowToolbar();

        uiManager.NavigateTo(PAGE::SVCONNECTPAGE);
    }

private:
    static Application* app;
    UIManager uiManager;
};

Application* Application::app = 0;