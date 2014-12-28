#pragma once

#include "client/UI/Page.h"

class SVConnectPage: public Page
{
public:
    void OnControlEvent(Control* control, int eventID)
    {
        
    }
    SVConnectPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::SVCONNECTPAGE;
        this->Initialize(parentWindow, fixed);

        int x = 300, y = 100;

        this->AddLabel("<span font='22'>Connect to a Server</span>", x, y);
        this->AddLabel("<span font='14'>Server address:</span>", x, y + 100);
        this->AddTextEdit(x, y + 140, 350, 30);
        this->AddButton("Connect", x, y + 240, 350, 30);
    }
private:
};