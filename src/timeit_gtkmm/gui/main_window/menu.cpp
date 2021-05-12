/*
 * menu.cpp
 *
 *  Created on: 2008-aug-20
 *      Author: hoglet
 */

#include "menu.h"
#include <glibmm/i18n.h>

namespace gui
{

Menu::Menu()
{
	file_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-preferences"), sigc::mem_fun(this, &Menu::on_menu_preferences)));
	file_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-quit"), sigc::mem_fun(this, &Menu::on_menu_quit)));

	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Start"), sigc::mem_fun(this, &Menu::on_menu_start)));
	task_id_dependent_menus.push_back(&(task_menu.items().back()));

	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Stop"), sigc::mem_fun(this, &Menu::on_menu_stop)));
	task_id_dependent_menus.push_back(&(task_menu.items().back()));

	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all"), sigc::mem_fun(this, &Menu::on_menu_stop_all)));

	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Edit"), sigc::mem_fun(this, &Menu::on_menu_edit)));
	task_id_dependent_menus.push_back(&(task_menu.items().back()));

	//	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("Remove",
	//			sigc::mem_fun(this, &Menu::on_menu_remove)));
	//taskIDDependentMenus.push_back(m_taskMenu.items().back());
	task_menu.items().push_back(Gtk::Menu_Helpers::SeparatorElem());

	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Add time"), sigc::mem_fun(this, &Menu::on_menu_add_time)));
	task_id_dependent_menus.push_back(&(task_menu.items().back()));

	task_menu.items().push_back(Gtk::Menu_Helpers::SeparatorElem());

	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Add task"), sigc::mem_fun(this, &Menu::on_menu_add_task)));
	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Remove task"), sigc::mem_fun(this, &Menu::on_menu_remove_task)));
	task_id_dependent_menus.push_back(&(task_menu.items().back()));

	help_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-about"), sigc::mem_fun(this, &Menu::on_menu_about)));
	help_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Report bug"), sigc::mem_fun(this, &Menu::on_menu_report_bug)));
	help_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-help"), sigc::mem_fun(this, &Menu::on_menu_help)));

	items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), file_menu));

	items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Task"), task_menu));

	items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Help"), help_menu));
	set_task_is_selected(false);
}


void Menu::set_task_is_selected(bool show)
{
	for (auto* menu_item: task_id_dependent_menus)
	{
		menu_item->set_sensitive(show);
	}

}

void Menu::on_menu_about()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_about();
	}
}

void Menu::on_menu_help()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_help();
	}
}

void Menu::on_menu_add_task()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_add_task();
	}
}

void Menu::on_menu_remove_task()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_remove_task();
	}
}

void Menu::on_menu_add_time()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_add_time();
	}
}
void Menu::on_menu_quit()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_quit();
	}
}

void Menu::on_menu_start()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_start_task();
	}
}
void Menu::on_menu_stop()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_stop_task();
	}
}

void Menu::on_menu_stop_all()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_stop_timers();
	}
}

void Menu::on_menu_edit()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_edit_task();
	}
}
void Menu::on_menu_preferences()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_preferences();
	}
}
void Menu::on_menu_report_bug()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_report_bug();
	}
}



void Menu::attach(Action_observer* observer)
{
	observers.push_back(observer);
}
void Menu::detach(Action_observer* observer)
{
	observers.remove(observer);
}

}
