#pragma once

#include "client/UI/Page.h"
#include "client/UI/pages/pages.h"


class HomePage : public Page
{
public:
    Label* msgHistory;
    void OnControlEvent(Control* control, int eventID)
    {
    }
    void ClearMsgText()
    {
        TextEdit* msg = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::HP_MSGTEXT));
        msg->Clear();
    }
    std::string GetMsg()
    {
        TextEdit* msg = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::HP_MSGTEXT));
        return std::string(msg->GetText());
    }
    HomePage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::HOMEPAGE;
        this->Initialize(parentWindow, fixed);

        
        int x = 150, y = 100;
        this->AddTextEdit(PAGECONTROL::HP_MSGTEXT, x, y + 410, 590, 40);
        this->AddButton(PAGECONTROL::HP_SENDBUTTON, "Send", x + 600, y + 410, 100, 25);
        msgHistory = static_cast<Label*>(this->AddLabel(PAGECONTROL::HP_MSGHISTORYLABEL, x, y, 700, 400));
    }
};