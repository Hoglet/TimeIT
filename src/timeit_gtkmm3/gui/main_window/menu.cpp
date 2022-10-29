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

menu_bar::menu_bar()
{
	//ToDo file_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-preferences"), sigc::mem_fun(this, &menu_bar::on_menu_preferences)));
	//ToDo file_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-quit"), sigc::mem_fun(this, &menu_bar::on_menu_quit)));

	//ToDo 	task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Start"), sigc::mem_fun(this, &menu_bar::on_menu_start)));
	//ToDo task_id_dependent_menus.push_back(&(task_menu.items().back()));

	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Stop"), sigc::mem_fun(this, &menu_bar::on_menu_stop)));
	//ToDo task_id_dependent_menus.push_back(&(task_menu.items().back()));

	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all"), sigc::mem_fun(this, &menu_bar::on_menu_stop_all)));

	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Edit"), sigc::mem_fun(this, &menu_bar::on_menu_edit)));
	//ToDo task_id_dependent_menus.push_back(&(task_menu.items().back()));

	//	m_taskMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("Remove",
	//			sigc::mem_fun(this, &Menu::on_menu_remove)));
	//taskIDDependentMenus.push_back(m_taskMenu.items().back());
	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::SeparatorElem());

	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Add time"), sigc::mem_fun(this, &menu_bar::on_menu_add_time)));
	//ToDo task_id_dependent_menus.push_back(&(task_menu.items().back()));

	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::SeparatorElem());

	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Add task"), sigc::mem_fun(this, &menu_bar::on_menu_add_task)));
	//ToDo task_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Remove task"), sigc::mem_fun(this, &menu_bar::on_menu_remove_task)));
	//ToDo task_id_dependent_menus.push_back(&(task_menu.items().back()));

	//ToDo help_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-about"), sigc::mem_fun(this, &menu_bar::on_menu_about)));
	//ToDo help_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(_("Report bug"), sigc::mem_fun(this, &menu_bar::on_menu_report_bug)));
	//ToDo help_menu.items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-help"), sigc::mem_fun(this, &menu_bar::on_menu_help)));

	//ToDo items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), file_menu));

	//ToDo items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Task"), task_menu));

	//ToDo items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Help"), help_menu));
	set_task_is_selected(false);
}


void menu_bar::set_task_is_selected(bool show)
{
	for (auto* menu_item: task_id_dependent_menus)
	{
		menu_item->set_sensitive(show);
	}

}

void menu_bar::on_menu_about()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_about();
	}
}

void menu_bar::on_menu_help()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_help();
	}
}

void menu_bar::on_menu_add_task()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_add_task();
	}
}

void menu_bar::on_menu_remove_task()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_remove_task();
	}
}

void menu_bar::on_menu_add_time()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_add_time();
	}
}
void menu_bar::on_menu_quit()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_quit();
	}
}

void menu_bar::on_menu_start()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_start_task();
	}
}
void menu_bar::on_menu_stop()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_stop_task();
	}
}

void menu_bar::on_menu_stop_all()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_stop_timers();
	}
}

void menu_bar::on_menu_edit()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_edit_task();
	}
}
void menu_bar::on_menu_preferences()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_preferences();
	}
}
void menu_bar::on_menu_report_bug()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_report_bug();
	}
}



void menu_bar::attach(action_observer* observer)
{
	observers.push_back(observer);
}
void menu_bar::detach(action_observer* observer)
{
	observers.remove(observer);
}

}
