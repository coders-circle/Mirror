#include "client/UI/UIManager.h"

UIManager* UIManager::m_uiManager = 0;

void UIManager::MenuItemEventHandler(GtkWidget* widget, gpointer data)
{
    MenuBar::MenuEventData *eventData = static_cast<MenuBar::MenuEventData*>(data);
    
    switch (eventData->menuID)
    {
    case MENU::CONNECTIONSMENU:
        if (eventData->menuItemID == 0) std::cout << "So, connect to a server eh!";
        else std::cout << "You want to disconnect?? :/";
        break;
    case MENU::TOOLSMENU:
        std::cout << "What do you prefer? :v";
        break;
    case MENU::HELPMENU:
        m_uiManager->NavigateTo(PAGE::ABOUTPAGE);
        break;
    }
    std::cout << std::endl;
}


void UIManager::PageEventHandler(ControlEventData* eventData)
{
    if (eventData->page->GetID() == PAGE::LOGINPAGE)
        m_uiManager->NavigateTo(PAGE::SIGNUPPAGE);
    else
        m_uiManager->NavigateTo(PAGE::LOGINPAGE);
}



void UIManager::Initialize(GtkWidget* parent, GtkWidget* fixed)
{
    m_parent = parent;
    m_fixed = fixed;
    m_menubar.Initialize(parent);
    m_menubar.SetEventHandler(G_CALLBACK(MenuItemEventHandler));

    // Important!
    // The order of creation for menu and page,
    // should match the enum for MENU & PAGE respectively

    m_menubar.AddMenu("Connections");
    m_menubar.AddMenuItem(MENU::CONNECTIONSMENU, "Connect to a server");
    m_menubar.AddMenuItem(MENU::CONNECTIONSMENU, "Disconnect");
    m_menubar.AddMenu("Tools");
    m_menubar.AddMenuItem(MENU::TOOLSMENU, "Preferences");
    m_menubar.AddMenu("Help");
    m_menubar.AddMenuItem(MENU::HELPMENU, "About");
    m_menubar.FixedPut(fixed);
    m_menubar.Show();

    m_pages.resize(3);
    m_pages[0] = new LoginPage(m_parent, m_fixed);
    LoginPage* loginPage = static_cast<LoginPage*>(m_pages[0]);
    loginPage->SetEventHandler(PageEventHandler);
    m_pages[1] = new SignupPage(m_parent, m_fixed);
    SignupPage* signupPage = static_cast<SignupPage*>(m_pages[1]);
    signupPage->SetEventHandler(PageEventHandler);
    m_pages[2] = new AboutPage(m_parent, m_fixed);
    AboutPage* aboutPage = static_cast<AboutPage*>(m_pages[2]);
    aboutPage->SetEventHandler(PageEventHandler);
}

void UIManager::NavigateTo(int page)
{
    if (m_currentPage)
    {
        m_currentPage->HideControls();
    }
    m_currentPage = m_pages[page];
    m_currentPage->ShowControls();
}

UIManager::UIManager()
{
    if (m_uiManager)
    {
        throw "can't create multiple objects of the UIManager class";
    }
    m_uiManager = this;
    m_parent = 0;
    m_fixed = 0;
    m_currentPage = 0;
}