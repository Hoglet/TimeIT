#include "TaskList.h"
#include <string>
#include <glibmm/i18n.h>
#include <libtimeit/Utils.h>

namespace GUI
{
using namespace libtimeit;
using namespace std;
using namespace Gtk;
using namespace Glib;
TaskList::TaskList(shared_ptr<IDatabase> &database) :
		taskAccessor(database->getExtendedTaskAccessor())
{
	runningIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(Utils::getImagePath(), "running.svg"), 24, 24, true);
	blankIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(Utils::getImagePath(), "blank.svg"), 24, 24, true);
	treeModel = TreeStore::create(columns);
	set_model(treeModel);
	append_column(_("Name"), columns.col_name);
	append_column("", columns.col_pixbuf);
	append_column(_("Time"), columns.col_time);
	Gtk::TreeView::Column *pColumn = get_column(1);
	pColumn->set_min_width(60);
	treeModel->set_sort_column(columns.col_name, Gtk::SORT_ASCENDING); // Initial sorting column
	taskAccessor->attach(this);
	get_selection()->signal_changed().connect(sigc::mem_fun(*this, &TaskList::on_selection_changed));
	populate();

	//Popup menu
	Gtk::Menu::MenuList &menulist = Menu_Popup.items();

	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Start"), sigc::mem_fun(*this, &TaskList::on_menu_start)));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop"), sigc::mem_fun(*this, &TaskList::on_menu_stop)));
	menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Add time"), sigc::mem_fun(*this, &TaskList::on_menu_add_time)));
	menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Add task"), sigc::mem_fun(*this, &TaskList::on_menu_add_task)));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Edit task"), sigc::mem_fun(*this, &TaskList::on_menu_edit)));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Remove task"), sigc::mem_fun(*this, &TaskList::on_menu_remove_task)));

}

TaskList::~TaskList()
{
	int64_t selectedID = 0;
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_task_selection_changed(selectedID);
	}
	taskAccessor->detach(this);
}

bool TaskList::on_button_press_event(GdkEventButton *event)
{
	bool retval = TreeView::on_button_press_event(event);
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		Menu_Popup.popup(event->button, event->time);
		retval = true; //It has been handled.
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		on_menu_edit();
		retval = true;
	}
	return retval;
}

void TaskList::on_row_expanded(const TreeModel::iterator &iter, const TreeModel::Path &path)
{
	TreeModel::Row row = *iter;
	int id = row[columns.col_id];
	taskAccessor->setTaskExpanded(id, true);
}
void TaskList::on_row_collapsed(const TreeModel::iterator &iter, const TreeModel::Path &path)
{
	TreeModel::Row row = *iter;
	int id = row[columns.col_id];
	taskAccessor->setTaskExpanded(id, false);
}

void TaskList::on_taskAdded(int64_t taskID)
{
	doUpdate();
}

void TaskList::on_taskUpdated(int64_t taskID)
{
	shared_ptr<vector<ExtendedTask> > tasks = taskAccessor->getExtendedTask(taskID);
	if (tasks->size() > 0)
	{
		ExtendedTask &task = tasks->at(0);
		Gtk::TreeIter iter = findRow(taskID);
		if (iter != treeModel->children().end())
		{
			TreeModel::Row row = *iter;
			assignValuesToRow(row, task);
		}
		int64_t parentID = task.parentID();
		if (parentID > 0)
		{
			on_taskUpdated(parentID);
		}
	}
}

void TaskList::on_taskNameChanged(int64_t taskID)
{
	on_taskUpdated(taskID);
}

void TaskList::on_taskTimeChanged(int64_t taskID)
{
	on_taskUpdated(taskID);
}

void TaskList::on_taskRemoved(int64_t taskID)
{
	Gtk::TreeIter iter = findRow(taskID);
	if (iter != treeModel->children().end())
	{
		treeModel->erase(iter);
	}
}

void TaskList::on_selection_changed()
{
	int64_t selectedID = getSelectedID();
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_task_selection_changed(selectedID);
	}
}

void TaskList::on_taskParentChanged(int64_t)
{
	doUpdate();
}

void TaskList::on_completeUpdate()
{
	doUpdate();
}

void TaskList::doUpdate()
{
	empty();
	populate();
}

void TaskList::empty()
{
	treeModel->clear();
}
int64_t TaskList::getSelectedID()
{
	int retVal = 0;
	RefPtr<TreeSelection> refTreeSelection = get_selection();
	TreeModel::iterator iter;
	iter = refTreeSelection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row = *iter;
		retVal = row[columns.col_id];
		//Do something with the row.
	}
	return retVal;
}

Gtk::TreeModel::iterator TaskList::findRow(int id)
{
	TreeModel::Children children = treeModel->children();
	return subSearch(id, children);
}

Gtk::TreeModel::iterator TaskList::subSearch(int id, TreeModel::Children children)
{
	TreeIter iter;

	for (iter = children.begin(); iter != children.end(); iter++)
	{
		TreeModel::Row row = *iter;
		if (row.children().size() > 0)
		{
			TreeIter subIter = subSearch(id, row.children());
			if (subIter != row.children().end())
			{
				iter = subIter;
				break;
			}
		}
		if (row[columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

void TaskList::assignValuesToRow(TreeModel::Row &row, const ExtendedTask &task)
{
	row[columns.col_id] = task.ID();
	if (task.running())
	{
		row[columns.col_pixbuf] = runningIcon;
	}
	else
	{
		row[columns.col_pixbuf] = blankIcon;
	}
	row[columns.col_name] = task.name();
	time_t totalTime = task.totalTime();
	if (totalTime > 0)
	{
		row[columns.col_time] = Utils::seconds2hhmm(totalTime);
	}
	else
	{
		row[columns.col_time] = "";
	}
}

void TaskList::populate(TreeModel::Row *parent, int parentID)
{
	shared_ptr<vector<ExtendedTask>> tasks = taskAccessor->getExtendedTasks(parentID);

	for (int i = 0; i < (int) tasks->size(); i++)
	{
		TreeModel::Row row;
		TreeModel::iterator iter;
		if (parent)
		{
			iter = treeModel->append(parent->children());
		}
		else
		{
			iter = treeModel->append();
		}
		row = *iter;
		assignValuesToRow(row, tasks->at(i));
		populate(&row, tasks->at(i).ID());
		if (tasks->at(i).expanded())
		{
			TreeModel::Path path(iter);
			this->expand_to_path(path);
		}
	}
}

void TaskList::on_menu_start()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_start_task();
	}
}
void TaskList::on_menu_stop()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_stop_task();
	}
}
void TaskList::on_menu_edit()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_edit_task();
	}
}
void TaskList::on_menu_add_time()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_add_time();
	}
}
void TaskList::on_menu_add_task()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_add_task();
	}
}

void TaskList::on_menu_remove_task()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver *observer = *iter;
		observer->on_action_remove_task();
	}
}

void TaskList::attach(IActionObserver *observer)
{
	observers.push_back(observer);
}
void TaskList::detach(IActionObserver *observer)
{
	observers.remove(observer);
}
}
