#pragma once

#include <vector>
#include <string>
#include <gtk/gtk.h>


class MenuItem
{
public:
    MenuItem() :m_handle(0){}
    void Create(const std::string& label)
    {
        m_handle = gtk_menu_item_new_with_label(label.c_str());
    }
    GtkWidget* GetHandle()
    {
        return m_handle;
    }
private:
    GtkWidget* m_handle;
};

class Menu
{
public:
    Menu() :m_handle(0), m_menu(0){}
    void Create(const std::string& label)
    {
        m_handle = gtk_menu_new();
        m_menu = gtk_menu_item_new_with_label(label.c_str());
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menu), m_handle);
    }
    int AddItem(const std::string& label)
    {
        m_items.resize(m_items.size() + 1);
        m_items[m_items.size() - 1].Create(label);
        //gtk_menu_attach(GTK_MENU(m_handle), m_items[m_items.size() - 1].GetHandle(), 0, 0, 0, 0);
        gtk_menu_shell_append(GTK_MENU_SHELL(m_handle), m_items[m_items.size() - 1].GetHandle());
        return (m_items.size() - 1);
    }
    GtkWidget* GetItemHandle(int index)
    {
        return m_items[index].GetHandle();
    }
    GtkWidget* GetHandle()
    {
        return m_handle;
    }
    GtkWidget* GetMenu()
    {
        return m_menu;
    }
private:
    GtkWidget* m_handle;
    GtkWidget* m_menu;
    std::vector<MenuItem> m_items;
};

class MenuBar
{
public:
    struct MenuEventData
    {
        int menuID;
        int menuItemID;
        void* uiManager;
        MenuEventData(int menuID, int menuItemID, void* uiManager) :menuID(menuID),
            menuItemID(menuItemID), uiManager(uiManager){}
    };
    MenuBar() :m_handle(0), m_parent(0), m_menuEventHandler(0){}
    void FixedPut(GtkWidget* fixed)
    {
        gtk_fixed_put(GTK_FIXED(fixed), m_handle, 0, 0);
    }
    void Initialize(GtkWidget* parent)
    {
        m_parent = parent;
        m_handle = gtk_menu_bar_new();
        //gtk_container_add(GTK_CONTAINER(m_parent), m_handle);
    }
    void AddMenu(std::string label)
    {
        m_menus.resize(m_menus.size() + 1);
        m_menus[m_menus.size() - 1].Create(label);
        gtk_menu_shell_append(GTK_MENU_SHELL(m_handle), m_menus[m_menus.size() - 1].GetMenu());
    }
    void AddMenuItem(unsigned int menuID, std::string itemLabel)
    {
        
        if (menuID >= 0 && menuID < m_menus.size())
        {
            int itemID = m_menus[menuID].AddItem(itemLabel);
            g_signal_connect(G_OBJECT(m_menus[menuID].GetItemHandle(itemID)),
                "activate", m_menuEventHandler, new MenuEventData(menuID, itemID, m_eventData));
        }
        else
        {
            throw "invalid menu ID";
        }
    }
    void Show()
    {
        gtk_widget_show_all(m_handle);
    }
    void SetEventHandler(GCallback eventHandler, void* data)
    {
        m_menuEventHandler = eventHandler;
        m_eventData = data;
    }
private:
    GtkWidget* m_handle;
    GtkWidget* m_parent;
    std::vector<Menu> m_menus;
    GCallback m_menuEventHandler;
    void* m_eventData;
};