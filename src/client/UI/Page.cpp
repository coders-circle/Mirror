#include "client/UI/Page.h"



void Page::LoadFromFile(std::string fileName)
{
    FILE* fp = fopen(fileName.c_str(), "r");
}

void Page::AddButton(std::string label, int x, int y, int w, int h)
{
    m_controls.resize(m_controls.size() + 1);
    m_controls[m_controls.size() - 1] = new Button();
    ((Button*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
}
void Page::AddLabel(std::string label, int x, int y, int w, int h)
{
    m_controls.resize(m_controls.size() + 1);
    m_controls[m_controls.size() - 1] = new Label();
    ((Label*)m_controls[m_controls.size() - 1])->Set(label, m_fixed, x, y, w, h);
}
void Page::AddTextEdit(int x, int y, int w, int h)
{
    m_controls.resize(m_controls.size() + 1);
    m_controls[m_controls.size() - 1] = new TextEdit();
    ((TextEdit*)m_controls[m_controls.size() - 1])->Set(m_fixed, x, y, w, h);
}

void Page::ShowControls()
{
    for (int i = 0, lim = m_controls.size(); i < lim; i++)
    {
        m_controls[i]->Show();
    }
}

void Page::Initialize(GtkWidget* parentWindow, GtkWidget* fixed)
{
    m_parentWindow = parentWindow;
    m_fixed = fixed;
    //gtk_container_add(GTK_CONTAINER(m_parentWindow), m_fixed);
}