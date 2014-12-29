#ifndef __CONTROL__
#define __CONTROL__

#include <gtk/gtk.h>
#include <string>

enum CONTROL{ BUTTON, LABEL, TEXTEDIT, STATUSBAR};

// base class for UI control elements
class Control
{
public:
    Control() :m_handle(0){}
    void SetID(int id);
    int GetID();
    void PutFixedAt(GtkWidget *fixed, int x, int y);
    void PutFixedRelativeTo(Control* obj, GtkWidget* fixed, int xOffset, int yOffset);
    void Show();
    void Hide();
    GtkWidget* GetHandle();
protected:
    GtkWidget* m_handle;
    int m_x, m_y, m_w, m_h;
    std::string m_label;
    int m_type;
    int m_id;
};

#endif