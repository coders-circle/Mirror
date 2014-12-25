#pragma once

#pragma once

#include "client/UI/Page.h"


class SignupPage : public Page
{
public:
    int m_signupButton;
    int m_usernameText;
    int m_passwordText;
    int m_confirmPasswordText;
    void OnControlEvent(Control* control, int eventID)
    {
        if (control->GetID() == m_signupButton)
        {
            TextEdit* username = static_cast<TextEdit*>(this->GetControlByID(m_usernameText));
            std::cout << "So " << username->GetText() << ", you want to Signup eh?" << std::endl;
        }
    }
    SignupPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::SIGNUPPAGE;

        this->Initialize(parentWindow, fixed);
        this->AddLabel("<span font='22'>Welcome to</span>", 150, 100);
        this->AddLabel("<span font='16'>The Mirrors</span>", 150, 150);

        this->AddLabel("<span font='18'>Sign Up</span>", 450, 80);
        this->AddLabel("<span font='14'>Username:</span>", 450, 148);
        this->AddLabel("<span font='14'>Password:</span> ", 450, 218);
        this->AddLabel("<span font='14'>Confirm:</span> ", 450, 288);
        m_usernameText = this->AddTextEdit(600, 150, 200, 25);
        m_passwordText = this->AddTextEdit(600, 220, 200, 25);
        m_confirmPasswordText = this->AddTextEdit(600, 290, 200, 25);
        ((TextEdit*)this->GetControlByID(m_passwordText))->SetPasswordMode();
        m_signupButton = this->AddButton("Sign Up", 460, 380, 340, 35);
    }
};