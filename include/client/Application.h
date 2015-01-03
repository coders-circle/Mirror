#pragma once

#include "common/common.h"

#include "client/UI/UIManager.h"
#include "client/Client.h"

#include "client/UI/pages/LoginPage.h"
#include "client/UI/pages/SignupPage.h"
#include "client/UI/pages/AboutPage.h"
#include "client/UI/pages/SVConnectPage.h"
#include "client/UI/pages/HomePage.h"
#include "client/UI/pages/WaitPage.h"

#include <gdk/gdkkeysyms.h>


// A specific class to handle overall operations
class Application
{
public:
    enum MENU{ CONNECTIONS = 0, TOOLS, HELP };
    enum MENUITEM{ SVCONNECT = 0, DISCONNECT, PREFERENCE, ABOUT };
    enum TOOLITEM{ BACK = 0 };

    Application();
    void OnMenuItemClick(int menuItemID);
    void OnButtonEvent(int buttonID);
    void OnToolItemClick(int toolItemID);
    static void UIEventHandler(UIEventData* eventData);
    static void ClientMessageEventHandler(boost::shared_ptr<MessageEventData> eventData);
    static gboolean KeyPressHandler(GtkWidget* widget, GdkEventKey *keyEvent, gpointer data);
    void Initialize(GtkWidget* parent, GtkWidget* fixed);
    static gboolean ConnectionTest(gpointer data);

private:
    LoginPage*      loginPage;
    AboutPage*      aboutPage;
    SVConnectPage*  svConnectPage;
    HomePage*       homePage;
    WaitPage*       waitPage;

    static Application* app;

    UIManager   uiManager;
    Client      client;

    bool connectedtoSV;
    bool connectionThreadEnded;
    size_t connectionID;
};

