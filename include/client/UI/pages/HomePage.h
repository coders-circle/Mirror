#pragma once

#include "client/UI/Page.h"
#include "client/UI/pages/pages.h"
#include "client/MediaStream/VideoPlayback.h"


class HomePage : public Page
{
public:
    VideoPlayback videoPlayback;
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
    void ShowControls()
    {
        Page::ShowControls();
        videoPlayback.StartPlaybackAsync();
    }
    void HideControls()
    {
        Page::HideControls();
        videoPlayback.StopPlayback();
    }
    HomePage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::HOMEPAGE;
        this->Initialize(parentWindow, fixed);
        
        int x = 10, y = 10;
        videoPlayback.Set(fixed, x, y, 800, 600);

        //this->AddTextEdit(PAGECONTROL::HP_MSGTEXT, x, y + 340, 400, 40);
        //this->AddButton(PAGECONTROL::HP_SENDBUTTON, "Send", x + 410, y + 340, 90, 40);
        //msgHistory = static_cast<Label*>(this->AddLabel(PAGECONTROL::HP_MSGHISTORYLABEL, x, y, 500, 320));
    }
};