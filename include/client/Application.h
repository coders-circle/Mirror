#pragma once

#include "client/UI/UIManager.h"
#include "client/Client.h"

#include "client/UI/pages/LoginPage.h"
#include "client/UI/pages/SignupPage.h"
#include "client/UI/pages/AboutPage.h"
#include "client/UI/pages/SVConnectPage.h"
#include "client/UI/pages/HomePage.h"

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
        case EVENT::BUTTONCLICK:
            if (eventData->controlID == app->svConnectPage->connectButton)
            {
                app->client.SetName(app->svConnectPage->GetName());
                app->client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(app->svConnectPage->GetIP()), 10011));
                app->client.JoinChat(0);
                app->uiManager.NavigateTo(PAGE::HOMEPAGE);
                app->client.SetMessageEventHandler((ClientMessageEventHandler));
                app->client.HandleRequestsAsync();
            }
            else if (eventData->controlID == app->homePage->sendBttn)
            {
                app->client.SendMessage(0, app->homePage->GetMsg());
                app->homePage->msgHistory->AppendToNewLine(std::string("You: ") + app->homePage->GetMsg());
            }
            break;
        default:
            break;
        }
        std::cout << "Some shit just happened";
    }
    static void ClientMessageEventHandler(boost::shared_ptr<MessageEventData> eventData)
    {
        char temp[128];
        sprintf(temp, "%s: %s", eventData->senderId.c_str(), eventData->message.c_str());
        app->homePage->msgHistory->AppendToNewLine(std::string(temp));
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

        loginPage = new LoginPage(parent, fixed);
        aboutPage = new AboutPage(parent, fixed);
        svConnectPage = new SVConnectPage(parent, fixed);
        homePage = new HomePage(parent, fixed);

        uiManager.AddPage(loginPage);
        uiManager.AddPage(aboutPage);
        uiManager.AddPage(svConnectPage);
        uiManager.AddPage(homePage);

        

        uiManager.AddToolItemFromStock(GTK_STOCK_GO_BACK, TOOLITEM::BACK);

        uiManager.ShowMenu();
        uiManager.ShowToolbar();

        uiManager.NavigateTo(PAGE::SVCONNECTPAGE);
    }

private:
    LoginPage* loginPage;
    AboutPage* aboutPage;
    SVConnectPage* svConnectPage;
    HomePage* homePage;
    static Application* app;
    UIManager uiManager;
    Client client;
};

Application* Application::app = 0;
