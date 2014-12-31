#pragma once

#include "client/UI/pages/pages.h"

class WaitPage : public Page
{
public:
    WaitPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::WAITPAGE;
        this->Initialize(parentWindow, fixed);
        
        int w = this->GetParentWidth(), h = this->GetParentHeight();
        int x = w/2 - 200, y = 200;
        m_spinner = static_cast<Spinner*>(this->AddSpinner(x + 5, y + 10, 30, 30));
        this->AddLabel("<span font='22'>Connecting...</span>", x + 50, y);
        this->AddLabel("<span font='16'>Taking you to the Mirrors, please wait!</span>", x, y + 100);
        //spinner->Start();
    }
    void ShowControls()
    {
        Page::ShowControls();
        m_spinner->Start();
    }
    void HideControls()
    {
        m_spinner->Stop();
        Page::HideControls();
    }
private:
    Spinner* m_spinner;
};