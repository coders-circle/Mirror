#include "client/Application.h"


Application* Application::app = 0;

Application::Application()
{
    if (app)
    {
        throw "only 1 instance is allowed";
    }
    app = this;
    this->connectedtoSV = false;
    this->connectionThreadEnded = false;
    this->connectionID = 0;
}

void Application::Initialize(GtkWidget* parent, GtkWidget* fixed)
{
    g_signal_connect(G_OBJECT(parent), "key_press_event", G_CALLBACK(KeyPressHandler), NULL);

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
    waitPage = new WaitPage(parent, fixed);

    uiManager.AddPage(loginPage);
    uiManager.AddPage(aboutPage);
    uiManager.AddPage(svConnectPage);
    uiManager.AddPage(homePage);
    uiManager.AddPage(waitPage);

    uiManager.AddToolItemFromStock(GTK_STOCK_GO_BACK, TOOLITEM::BACK);

    uiManager.ShowMenu();
    uiManager.ShowToolbar();

    uiManager.NavigateTo(PAGE::SVCONNECTPAGE);

    
}

gboolean Application::ConnectionTest(gpointer data)
{
    
    if (!app->connectionThreadEnded)
    {
        return TRUE;
    }
        
    /*try
    {
        app->client.SetName(app->svConnectPage->GetName());
        app->connectionID = app->client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(app->svConnectPage->GetIP()), 10011));
    }
    catch (Exception err)
    {
        app->uiManager.NavigateBack();
    }*/

    if (app->client.IsConnected(app->connectionID))
    {
        app->client.SetServer(app->connectionID);
        app->client.JoinChat(app->connectionID);
        app->client.SetMessageEventHandler((ClientMessageEventHandler));
        app->client.HandleRequestsAsync();
        app->uiManager.NavigateTo(PAGE::HOMEPAGE);
    }
    else
    {
        app->uiManager.NavigateBack();
    }

    return FALSE;
}



void Application::OnMenuItemClick(int menuItemID)
{
    switch (menuItemID)
    {
    case MENUITEM::SVCONNECT:
        app->uiManager.NavigateTo(PAGE::SVCONNECTPAGE);
        break;
    case MENUITEM::DISCONNECT:
        break;
    case MENUITEM::PREFERENCE:
        break;
    case MENUITEM::ABOUT:
        app->uiManager.NavigateTo(PAGE::ABOUTPAGE);
        break;
    default:
        // may be throw something
        break;
        
    }
}
void Application::OnButtonEvent(int buttonID)
{
    switch (buttonID)
    {
    case PAGECONTROL::LP_LOGINBUTTON:
        break;
    case PAGECONTROL::SP_SIGNUPBUTTON:
        break;
    case PAGECONTROL::SCP_CONNECTBUTTON:
        if (this->connectedtoSV)
        {
            app->uiManager.NavigateTo(PAGE::HOMEPAGE);
            break;
        }
        try
        {
            
            app->client.ConnectAsync(tcp::endpoint(boost::asio::ip::address::from_string(app->svConnectPage->GetIP()), 10011),
                &(app->connectionThreadEnded), &(app->connectionID));
            app->uiManager.NavigateTo(PAGE::WAITPAGE);
            g_timeout_add(100, ConnectionTest, 0);
            //app->client.SetName(app->svConnectPage->GetName());
            //app->client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(app->svConnectPage->GetIP()), 10011));
            //app->client.JoinChat(0);
            //app->uiManager.NavigateTo(PAGE::HOMEPAGE);
            //app->client.SetMessageEventHandler((ClientMessageEventHandler));
            //app->client.HandleRequestsAsync();
            //this->connectedtoSV = true;
        }
        catch (std::exception err)
        {
            //std::cout << "oops! something bad happened\n" << err.what();
            //std::cout << std::endl;
        }
        break;
    case PAGECONTROL::HP_SENDBUTTON:
        app->client.SendMessage(app->connectionID, app->homePage->GetMsg());
        app->homePage->msgHistory->AppendToNewLine(std::string("You: ") + app->homePage->GetMsg());
        app->homePage->ClearMsgText();
        break;
    default:
        // may be throw something
        break;
        
    }
}
void Application::OnToolItemClick(int toolItemID)
{
    switch (toolItemID)
    {
    case TOOLITEM::BACK:
        app->uiManager.NavigateBack();
        break;
    default:
        // may be throw something
        break;
        
    }
}
void Application::UIEventHandler(UIEventData* eventData)
{
    switch (eventData->event)
    {
    case EVENT::MENUITEMCLICK:
        app->OnMenuItemClick(eventData->controlID);
        break;
    case EVENT::BUTTONCLICK:
        app->OnButtonEvent(eventData->controlID);
        break;
    case EVENT::TOOLITEMCLICK:
        app->OnToolItemClick(eventData->controlID);
        break;
    default:
        break;
    }
}
void Application::ClientMessageEventHandler(boost::shared_ptr<MessageEventData> eventData)
{
    char temp[128];
    sprintf(temp, "%s: %s", eventData->senderId.c_str(), eventData->message.c_str());
    app->homePage->msgHistory->AppendToNewLine(std::string(temp));
}

gboolean Application::KeyPressHandler(GtkWidget* widget, GdkEventKey *keyEvent, gpointer data)
{
    switch (keyEvent->keyval)
    {
    case GDK_KEY_Return:
        if (app->uiManager.GetCurrentPageID() == PAGE::HOMEPAGE)
        {
            app->OnButtonEvent(PAGECONTROL::HP_SENDBUTTON);
        }
        break;
    default:
        break;
    }
    return false;
}
