#pragma once

#include "client/UI/Page.h"


class HomePage : public Page
{
public:
    int msgText;
    int sendBttn;
    Label* msgHistory;
    void OnControlEvent(Control* control, int eventID)
    {
        TextEdit* msg = static_cast<TextEdit*>(this->GetControlByID(msgText));
        std::string tmp = "You: ";
        tmp += msg->GetText();
        msgHistory->AppendToNewLine(tmp);
        msg->Clear();
    }
    std::string GetMsg()
    {
        TextEdit* msg = static_cast<TextEdit*>(this->GetControlByID(msgText));
        return std::string(msg->GetText());
    }
    HomePage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::HOMEPAGE;
        this->Initialize(parentWindow, fixed);

        msgText = 100;
        sendBttn = 101;
        int x = 150, y = 100;
        this->AddTextEdit(msgText, x, y, 300, 40);
        this->AddButton(sendBttn, "Send", x + 320, y, 100, 25);
        msgHistory = static_cast<Label*>(this->AddLabel(2, x, y + 50));
    }
};