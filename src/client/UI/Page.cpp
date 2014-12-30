#include "client/UI/Page.h"

void Page::AllocateNewControl(int type, int id)
{
    m_controls.resize(m_controls.size() + 1);
    switch (type)
    {
    case CONTROL::BUTTON:   m_controls[m_controls.size() - 1] = new Button();       break;
    case CONTROL::LABEL:    m_controls[m_controls.size() - 1] = new Label();        break;
    case CONTROL::TEXTEDIT: m_controls[m_controls.size() - 1] = new TextEdit();     break;
    }
    m_controls[m_controls.size() - 1]->SetID(id);
}



Control* Page::GetControlByID(unsigned int controlID)
{
    int controlIndex = -1;
    for (int i = 0, lim = m_controls.size(); i < lim; i++)
    {
        if (m_controls[i]->GetID() == controlID)
        {
            controlIndex = i;
            break;
        }
    }
    if (controlIndex == -1) throw Exception("invalid value of controlID");
    return m_controls[controlIndex];
}



// TODO: may be design a file format for the page
void Page::LoadFromFile(std::string fileName)
{
    //FILE* fp = fopen(fileName.c_str(), "r");
}

Control* Page::AddButton(int id, std::string label, int x, int y, int w, int h)
{
    this->AllocateNewControl(CONTROL::BUTTON, id);
    //EventData ev(this, m_controls[m_controls.size() - 1], ControlEvent::BUTTON_CLICK);
    ((Button*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
    g_signal_connect(G_OBJECT(m_controls[m_controls.size() - 1]->GetHandle()), "clicked", 
        G_CALLBACK(ControlEventHandler), new ControlEventData(this, m_controls[m_controls.size() - 1], CONTROLEVENT::CLICKED));
    return m_controls[m_controls.size() - 1];
}

Control* Page::AddLabel(std::string label, int x, int y, int w, int h, int justification)
{
    this->AllocateNewControl(CONTROL::LABEL, -1);
    ((Label*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
    ((Label*)m_controls[m_controls.size() - 1])->SetJustify(justification);
    return m_controls[m_controls.size() - 1];
}

Control* Page::AddLabel(int id, int x, int y, int w, int h)
{
    this->AllocateNewControl(CONTROL::LABEL, id);
    ((Label*)m_controls[m_controls.size() - 1])->Set(id, m_fixed, x, y, w, h);
    return m_controls[m_controls.size() - 1];
}

Control* Page::AddTextEdit(int id, int x, int y, int w, int h)
{
    this->AllocateNewControl(CONTROL::TEXTEDIT, id);
    ((TextEdit*)m_controls[m_controls.size() - 1])->Set(m_fixed, x, y, w, h);
    return m_controls[m_controls.size() - 1];
}


void Page::ShowControls()
{
    for (int i = 0, lim = m_controls.size(); i < lim; i++)
    {
        m_controls[i]->Show();
    }
}

void Page::HideControls()
{
    for (int i = 0, lim = m_controls.size(); i < lim; i++)
    {
        m_controls[i]->Hide();
    }
}

void Page::Initialize(GtkWidget* parentWindow, GtkWidget* fixed)
{
    m_parentWindow = parentWindow;
    m_fixed = fixed;
}

void Page::OnControlEvent(Control*, int eventID)
{
}

void Page::ControlEventHandler(GtkWidget* widget, gpointer data)
{
    ControlEventData* eventData = static_cast<ControlEventData*> (data);
    Page* parentPage = eventData->page;
    Control* ctrl = eventData->control;
    parentPage->OnControlEvent(ctrl, eventData->eventID);
    (*(parentPage->m_pageEventHandler))(new ControlEventData(parentPage, ctrl, eventData->eventID));
}

void Page::SetEventHandler(PageEventCallBack eventHandler)
{
    m_pageEventHandler = eventHandler;
}

int Page::GetID()
{
    return m_ID;
}
