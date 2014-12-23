#include "client/UI/Page.h"

void Page::AllocateNewControl(Control::ControlType type)
{
    m_controls.resize(m_controls.size() + 1);
    switch (type)
    {
    case Control::BUTTON:   m_controls[m_controls.size() - 1] = new Button();   break;
    case Control::LABEL:    m_controls[m_controls.size() - 1] = new Label();    break;
    case Control::TEXTEDIT: m_controls[m_controls.size() - 1] = new TextEdit();   break;
    }
    m_controls[m_controls.size() - 1]->SetID(m_controls.size() - 1);
}



Control* Page::GetControlByID(unsigned int controlID)
{
    if (controlID >= 0 && controlID < m_controls.size())
    {
        return m_controls[controlID];
    }
    throw Exception("invalid value of controlID");
}



// TODO: may be design a file format for the page
void Page::LoadFromFile(std::string fileName)
{
    //FILE* fp = fopen(fileName.c_str(), "r");
}

int Page::AddButton(std::string label, int x, int y, int w, int h)
{
    this->AllocateNewControl(Control::BUTTON);
    //EventData ev(this, m_controls[m_controls.size() - 1], ControlEvent::BUTTON_CLICK);
    ((Button*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
    g_signal_connect(G_OBJECT(m_controls[m_controls.size() - 1]->GetHandle()), "clicked", 
        G_CALLBACK(ControlEventHandler), new ControlEventData(this, m_controls[m_controls.size() - 1], ControlEvent::BUTTON_CLICK));
    return m_controls[m_controls.size() - 1]->GetID();
}
int Page::AddLabel(std::string label, int x, int y, int w, int h)
{
    this->AllocateNewControl(Control::LABEL);
    ((Label*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
    return m_controls[m_controls.size() - 1]->GetID();
}
int Page::AddTextEdit(int x, int y, int w, int h)
{
    this->AllocateNewControl(Control::TEXTEDIT);
    ((TextEdit*)m_controls[m_controls.size() - 1])->Set(m_fixed, x, y, w, h);
    return m_controls[m_controls.size() - 1]->GetID();
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

void Page::OnControlEvent(int control_id)
{
}

void Page::ControlEventHandler(GtkWidget* widget, gpointer data)
{
    ControlEventData* eventData = static_cast<ControlEventData*> (data);
    Page* parentPage = eventData->page;
    Control* ctrl = eventData->control;
    parentPage->OnControlEvent(ctrl->GetID());
    (*(parentPage->m_pageEventHandler))(new ControlEventData(parentPage, ctrl, eventData->event_id));
}

void Page::SetEventHandler(PageEventCallBack eventHandler)
{
    m_pageEventHandler = eventHandler;
}

int Page::GetID()
{
    return m_ID;
}
