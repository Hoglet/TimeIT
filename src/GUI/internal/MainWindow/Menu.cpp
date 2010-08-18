/*
 * menu.cpp
 *
 *  Created on: 2008-aug-20
 *      Author: hoglet
 */

#include "Menu.h"
#include <glibmm/i18n.h>

namespace GUI
{
namespace Internal
{

Menu::Menu()
{
	m_fileMenu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-preferences"), sigc::mem_fun(this, &Menu::on_menu_preferences)));
	m_fileMenu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-quit"), sigc::mem_fun(this, &Menu::on_menu_quit)));

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Start"), sigc::mem_fun(this, &Menu::on_menu_start)));
	taskIDDependentMenus.push_back(&(m_taskMenu.items().back()));

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Stop"), sigc::mem_fun(this, &Menu::on_menu_stop)));
	taskIDDependentMenus.push_back(&(m_taskMenu.items().back()));

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all"), sigc::mem_fun(this, &Menu::on_menu_stop_all)));

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Edit"), sigc::mem_fun(this, &Menu::on_menu_edit)));
	taskIDDependentMenus.push_back(&(m_taskMenu.items().back()));

	//	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("Remove",
	//			sigc::mem_fun(this, &Menu::on_menu_remove)));
	//taskIDDependentMenus.push_back(m_taskMenu.items().back());
	m_taskMenu.items().push_back(Gtk::Menu_Helpers::SeparatorElem());

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Add time"), sigc::mem_fun(this, &Menu::on_menu_add_time)));
	taskIDDependentMenus.push_back(&(m_taskMenu.items().back()));

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::SeparatorElem());

	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Add task"), sigc::mem_fun(this, &Menu::on_menu_add_task)));
	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Remove task"), sigc::mem_fun(this, &Menu::on_menu_remove_task)));
	taskIDDependentMenus.push_back(&(m_taskMenu.items().back()));

	m_helpMenu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-about"), sigc::mem_fun(this, &Menu::on_menu_about)));
	m_helpMenu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Report bug"), sigc::mem_fun(this, &Menu::on_menu_report_bug)));
	m_helpMenu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-help"), sigc::mem_fun(this, &Menu::on_menu_help)));

	items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), m_fileMenu));

	items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Task"), m_taskMenu));

	items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Help"), m_helpMenu));
	setTaskIsSelected(false);
}

Menu::~Menu()
{
}

void Menu::setTaskIsSelected(bool show)
{
	std::list<Gtk::MenuItem*>::iterator iter;
	for (iter = taskIDDependentMenus.begin(); iter != taskIDDependentMenus.end(); iter++)
	{
		Gtk::MenuItem* menuItem = *iter;
		menuItem->set_sensitive(show);
	}

}

void Menu::on_menu_about()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_about();
	}
}

void Menu::on_menu_help()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_help();
	}
}

void Menu::on_menu_add_task()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_add_task();
	}
}

void Menu::on_menu_remove_task()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_remove_task();
	}
}

void Menu::on_menu_add_time()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_add_time();
	}
}
void Menu::on_menu_quit()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_quit();
	}
}

void Menu::on_menu_start()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_start_task();
	}
}
void Menu::on_menu_stop()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_stop_task();
	}
}

void Menu::on_menu_stop_all()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_stopTimers();
	}
}

void Menu::on_menu_edit()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_edit_task();
	}
}
void Menu::on_menu_preferences()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_preferences();
	}
}
void Menu::on_menu_report_bug()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_report_bug();
	}
}



void Menu::attach(IActionObserver* observer)
{
	observers.push_back(observer);
}
void Menu::detach(IActionObserver* observer)
{
	observers.remove(observer);
}

}
}
