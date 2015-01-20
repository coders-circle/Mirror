#pragma once

#include "client/UI/Page.h"
#include "client/UI/pages/pages.h"

class SVConnectPage: public Page
{
public:
    void OnControlEvent(Control* control, int eventID)
    {
        
    }
    std::string GetName()
    {
        TextEdit* name = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::SCP_NAMETEXT));
        return name->GetText();
    }
    std::string GetIP()
    {
        TextEdit* ip = static_cast<TextEdit*>(this->GetControlByID(PAGECONTROL::SCP_IPTEXT));
        return ip->GetText();
    }
    SVConnectPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::SVCONNECTPAGE;
        this->Initialize(parentWindow, fixed);

        int x = 300, y = 100;
        
        this->AddLabel("<span font='22'>Connect to a Server</span>", x, y);
        this->AddLabel("<span font='14'>Name:</span> ", x, y + 100);
        this->AddLabel("<span font='14'>Server address:</span>", x, y + 150);
        
        this->AddLabel("<span font='14'>Group ID:</span> ", x, y + 200);

        
        this->AddTextEdit(PAGECONTROL::SCP_NAMETEXT,    x + 150, y + 100, 200, 30);
        this->AddTextEdit(PAGECONTROL::SCP_IPTEXT,      x + 150, y + 150, 200, 30);
        this->AddTextEdit(PAGECONTROL::SCP_GROUPIDTEXT, x + 150, y + 200, 200, 30);

        this->AddButton(PAGECONTROL::SCP_CONNECTBUTTON, "Connect", x, y + 300, 350, 30);
    }
private:
};