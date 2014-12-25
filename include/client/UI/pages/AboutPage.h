#pragma once

#include "client/UI/Page.h"

class AboutPage: public Page
{
public:
    void OnControlEvent(Control* control, int eventID)
    {

    }
    AboutPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::ABOUTPAGE;
        this->Initialize(parentWindow, fixed);

        int sx = 150, sy = 100;

        this->AddLabel("<span font='14'>About</span>", sx, sy);
        this->AddLabel("<span font='20'>Mirrors</span>", sx + 140, sy + 80);
        this->AddLabel("<span font='13'>An awesome software written by group of awesome peoples</span>",
            sx + 140, sy + 120);
        this->AddLabel("<span font='13'><b>Mirrors</b>\n"
            "Copyright (c) 2015 Authors. All rights reserved (Just Kidding).\n"
            "This software is released under the <a href='http://www.gnu.org/copyleft/gpl.html#content'>GNU General Public License</a>.\n\n"
            "Mirrors is made possible by the <a href='http://www.boost.org'>Boost</a> and other open source libraries</span>",
            sx, sy + 250);

    }
private:
};