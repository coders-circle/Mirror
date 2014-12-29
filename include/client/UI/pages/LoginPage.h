#pragma once

#include "client/UI/Page.h"
#include "client/UI/pages/pages.h"


class LoginPage : public Page
{
public:
    void OnControlEvent(Control* control, int eventID)
    {
        if (control->GetID() == PAGECONTROL::LP_LOGINBUTTON)
        {
            TextEdit* username = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::LP_USERNAMETEXT));
            TextEdit* password = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::LP_PASSWORDTEXT));
            if (!(username->GetText()[0]&&password->GetText()[0]))
            {
                GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
                GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(m_parentWindow), 
                    flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE, "%s", "Enter your username and password first!");
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
            }
        }
    }
    LoginPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::LOGINPAGE;
        this->Initialize(parentWindow, fixed);
        
        this->AddTextEdit(PAGECONTROL::LP_USERNAMETEXT, 600, 150, 200, 25);
        this->AddTextEdit(PAGECONTROL::LP_PASSWORDTEXT, 600, 220, 200, 25);
        
        this->AddButton(PAGECONTROL::LP_LOGINBUTTON, "Log In", 460, 320, 340, 35);

        ((TextEdit*)this->GetControlByID(PAGECONTROL::LP_PASSWORDTEXT))->SetPasswordMode();


        this->AddLabel("<span font='22'>Welcome to</span>",     150, 100);
        this->AddLabel("<span font='16'>The Mirrors</span>",    150, 150);

        this->AddLabel("<span font='18'>Log in</span>",     450, 80);
        this->AddLabel("<span font='14'>Username:</span>",  450, 148);
        this->AddLabel("<span font='14'>Password:</span> ", 450, 218);
        
    }
};