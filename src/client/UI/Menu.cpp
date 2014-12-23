#include "client/UI/Menu.h"


////////////////////////////////////////////////////////////
// Definitions for member functions of MenuItem
////////////////////////////////////////////////////////////

void MenuItem::Create(const std::string& label)
{
    m_handle = gtk_menu_item_new_with_label(label.c_str());
}

GtkWidget* MenuItem::GetHandle()
{
    return m_handle;
}





////////////////////////////////////////////////////////////
// Definitions for member functions of Menu
////////////////////////////////////////////////////////////


void Menu::Create(const std::string& label)
{
    m_handle = gtk_menu_new();
    m_menu = gtk_menu_item_new_with_label(label.c_str());
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menu), m_handle);
}

int Menu::AddItem(const std::string& label)
{
    m_items.resize(m_items.size() + 1);
    m_items[m_items.size() - 1].Create(label);
    gtk_menu_shell_append(GTK_MENU_SHELL(m_handle), m_items[m_items.size() - 1].GetHandle());
    return (m_items.size() - 1);
}


GtkWidget* Menu::GetItemHandle(unsigned int index)
{
    if (index < 0 && index >= m_items.size())
    {
        throw Exception("invalid index for menu item");
    }
    return m_items[index].GetHandle();
}

GtkWidget* Menu::GetHandle()
{
    return m_handle;
}

GtkWidget* Menu::GetMenu()
{
    return m_menu;
}


////////////////////////////////////////////////////////////
// Definitions for member functions of MenuBar
////////////////////////////////////////////////////////////

void MenuBar::FixedPut(GtkWidget* fixed)
{
    gtk_fixed_put(GTK_FIXED(fixed), m_handle, 0, 0);
}

void MenuBar::Initialize(GtkWidget* parent)
{
    m_parent = parent;
    m_handle = gtk_menu_bar_new();
}

void MenuBar::AddMenu(std::string label)
{
    m_menus.resize(m_menus.size() + 1);
    m_menus[m_menus.size() - 1].Create(label);
    gtk_menu_shell_append(GTK_MENU_SHELL(m_handle), m_menus[m_menus.size() - 1].GetMenu());
}

void MenuBar::AddMenuItem(unsigned int menuID, std::string itemLabel)
{

    if (menuID >= 0 && menuID < m_menus.size())
    {
        int itemID = m_menus[menuID].AddItem(itemLabel);
        g_signal_connect(G_OBJECT(m_menus[menuID].GetItemHandle(itemID)),
            "activate", m_menuEventHandler, new MenuEventData(menuID, itemID));
    }
    else
    {
        throw Exception("invalid value for menuID");
    }
}

void MenuBar::Show()
{
    gtk_widget_show_all(m_handle);
}
void MenuBar::SetEventHandler(GCallback eventHandler)
{
    m_menuEventHandler = eventHandler;
}