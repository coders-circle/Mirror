#pragma once

#include "client/UI/Page.h"


class LoginPage : public Page
{
public:
    int m_testButton;
    int m_loginButton;
    int m_usernameText;
    int m_passwordText;
    void OnControlEvent(Control* control, int eventID)
    {
        if (control->GetID() == m_testButton)
            std::cout << "Okie, did somebody just pressed \"Hello World!\"?" << std::endl;
        else if (control->GetID() == m_loginButton)
        {
            
            TextEdit* username = static_cast<TextEdit*>(this->GetControlByID(m_usernameText));
            std::cout << "So " << username->GetText() <<", you want to Log-In eh?" << std::endl;
        }
    }
    LoginPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::LOGINPAGE;
        this->Initialize(parentWindow, fixed);
        this->AddLabel("<span font='22'>Welcome to</span>",     150, 100);
        this->AddLabel("<span font='16'>The Mirrors</span>",    150, 150);

        this->AddLabel("<span font='18'>Log in</span>",     450, 80);
        this->AddLabel("<span font='14'>Username:</span>",  450, 148);
        this->AddLabel("<span font='14'>Password:</span> ", 450, 218);
        m_usernameText = this->AddTextEdit(600, 150, 200, 25);
        m_passwordText = this->AddTextEdit(600, 220, 200, 25);
        ((TextEdit*)this->GetControlByID(m_passwordText))->SetPasswordMode();
        m_loginButton = this->AddButton("Log In", 460, 320, 340, 35);
    }
};