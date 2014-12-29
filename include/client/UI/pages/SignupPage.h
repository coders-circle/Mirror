#pragma once


#include "client/UI/Page.h"
#include "client/UI/pages/pages.h"


class SignupPage : public Page
{
public:
    void OnControlEvent(Control* control, int eventID)
    {
        if (control->GetID() == PAGECONTROL::SP_SIGNUPBUTTON)
        {
            TextEdit* username = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::SP_USERNAMETEXT));
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
        this->AddTextEdit(PAGECONTROL::SP_USERNAMETEXT, 600, 150, 200, 25);
        this->AddTextEdit(PAGECONTROL::SP_PASSWORDTEXT, 600, 220, 200, 25);
        this->AddTextEdit(PAGECONTROL::SP_CONFIRMPASSWORDTEXT,600, 290, 200, 25);
        ((TextEdit*)this->GetControlByID(PAGECONTROL::SP_PASSWORDTEXT))->SetPasswordMode();
        ((TextEdit*)this->GetControlByID(PAGECONTROL::SP_CONFIRMPASSWORDTEXT))->SetPasswordMode();
        this->AddButton(PAGECONTROL::SP_SIGNUPBUTTON, "Sign Up", 460, 380, 340, 35);
    }
};