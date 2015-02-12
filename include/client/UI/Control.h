#pragma once

#include <gtk/gtk.h>
#include <string>

enum CONTROL{ BUTTON, LABEL, TEXTEDIT, STATUSBAR, SPINNER, FRAMERENDERER};

// base class for UI control elements
class Control
{
public:
    Control();
    virtual void SetID(int id);
    virtual int ID();
    virtual int GetID();
    virtual void PutFixedAt(GtkWidget *fixedContainer, int x, int y);
    virtual void Show();
    virtual void Hide();
    virtual GtkWidget* GetHandle();

    virtual void MoveTo(int x, int y);
    virtual void Resize(int w, int h);

    void SetContainer(GtkWidget* container);

protected:
    GtkWidget* m_handle;
    GtkWidget* m_container;
    int m_x, m_y, m_w, m_h;
    std::string m_label;
    int m_type;
    int m_id;
};

