#pragma once

#include "client/UI/Page.h"


class LoginPage : public Page
{
public:
    int m_testButton;
    int m_loginButton;
    int m_usernameText;
    int m_passwordText;
    void OnControlEvent(int control_id)
    {
        if (control_id == m_testButton)
            std::cout << "Okie, did somebody just pressed \"Hello World!\"?" << std::endl;
        else if (control_id == m_loginButton)
        {
            
            TextEdit* username = static_cast<TextEdit*>(this->GetControlByID(m_usernameText));
            std::cout << "So " << username->GetText() <<", you want to Log-In eh?" << std::endl;
        }
    }
    LoginPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->Initialize(parentWindow, fixed);
        this->AddLabel("<span font='22'>Welcome to</span>",     150, 100, 200, 50);
        this->AddLabel("<span font='16'>The Mirrors</span>",    150, 150, 200, 50);

        this->AddLabel("<span font='18'>Log in</span>",     450, 80, 100, 50);
        this->AddLabel("<span font='14'>Username:</span>",  450, 150, 120, 30);
        this->AddLabel("<span font='14'>Password:</span> ", 450, 220, 120, 30);
        m_usernameText = this->AddTextEdit(600, 150, 200, 25);
        m_passwordText = this->AddTextEdit(600, 220, 200, 25);
        ((TextEdit*)this->GetControlByID(m_passwordText))->SetPasswordMode();
        m_loginButton = this->AddButton("Log In", 460, 320, 340, 35);
    }
};