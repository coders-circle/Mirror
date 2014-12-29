#pragma once

#include "client/UI/Page.h"

class SVConnectPage: public Page
{
    int m_connectButton;
    int m_ipText;
public:
    void OnControlEvent(Control* control, int eventID)
    {
        
    }
    SVConnectPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::SVCONNECTPAGE;
        this->Initialize(parentWindow, fixed);

        int x = 300, y = 100;
        m_connectButton = 0;
        m_ipText = 1;
        this->AddLabel("<span font='22'>Connect to a Server</span>", x, y);
        this->AddLabel("<span font='14'>Server address:</span>", x, y + 100);
        this->AddTextEdit(m_ipText, x, y + 140, 350, 30);
        this->AddButton(m_connectButton, "Connect", x, y + 240, 350, 30);
    }
private:
};