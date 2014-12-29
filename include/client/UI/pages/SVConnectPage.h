#pragma once

#include "client/UI/Page.h"

class SVConnectPage: public Page
{
public:
    int connectButton;
    int ipText;
    int nameText;
    int grpIDText;

    void OnControlEvent(Control* control, int eventID)
    {
        
    }
    std::string GetName()
    {
        TextEdit* name = static_cast<TextEdit*>(this->GetControlByID(nameText));
        return name->GetText();
    }
    std::string GetIP()
    {
        TextEdit* ip = static_cast<TextEdit*>(this->GetControlByID(ipText));
        return ip->GetText();
    }
    SVConnectPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::SVCONNECTPAGE;
        this->Initialize(parentWindow, fixed);

        int x = 300, y = 100;
        connectButton = 0;
        ipText = 1;
        nameText = 2;
        grpIDText = 3;

        
        this->AddLabel("<span font='22'>Connect to a Server</span>", x, y);
        this->AddLabel("<span font='14'>Name:</span> ", x, y + 100);
        this->AddLabel("<span font='14'>Server address:</span>", x, y + 150);
        
        this->AddLabel("<span font='14'>Group ID:</span> ", x, y + 200);

        
        this->AddTextEdit(nameText, x + 150, y + 100, 200, 30);
        this->AddTextEdit(ipText, x + 150, y + 150, 200, 30);
        this->AddTextEdit(grpIDText, x + 150, y + 200, 200, 30);

        this->AddButton(connectButton, "Connect", x, y + 300, 350, 30);
    }
private:
};