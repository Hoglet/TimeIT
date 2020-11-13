#include "Summary.h"
#include "libtimeit/Utils.h"
#include "DetailsDialog.h"
#include <glibmm/i18n.h>
using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace DB;
namespace GUI
{
SummaryObserver::SummaryObserver()
{
	unsubscription_allowed = true;
}
SummaryObserver::~SummaryObserver()
{
	unsubscription_allowed = false;
	std::list<ISummary*>::iterator iter = subjects.begin();
	while (iter != subjects.end())
	{
		ISummary *subject = *iter;
		subject->detach(this);
		++iter;
	}
}
void SummaryObserver::attach(ISummary *subject)
{
	subjects.push_back(subject);
}
void SummaryObserver::detach(ISummary *subject)
{
	if (unsubscription_allowed)
	{
		subjects.remove(subject);
	}
}

Summary::Summary(std::shared_ptr<DB::IDatabase> &database) :
		timeAccessor(database->getTimeAccessor()), taskAccessor(database->getTaskAccessor())
{
	treeModel = TreeStore::create(columns);
	set_model(treeModel);
	append_column("Name", columns.col_name);
	append_column("Time", columns.col_time);
	set_headers_visible(false);

	refTreeSelection = get_selection();
	refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &Summary::on_selection_changed));
	taskAccessor->attach(this);

	Gtk::Menu::MenuList &menulist = Menu_Popup.items();

	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Show details"), sigc::mem_fun(*this, &Summary::on_menu_showDetails)));

}

Summary::~Summary()
{
	taskAccessor->detach(this);
}

bool Summary::on_button_press_event(GdkEventButton *event)
{
	bool retVal = Gtk::TreeView::on_button_press_event(event);
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		Menu_Popup.popup(event->button, event->time);
		retVal = true; //It has been handled.
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		on_menu_showDetails();
		retVal = true;
	}
	return retVal;
}

void Summary::on_menu_showDetails()
{
	int64_t id = getSelectedID();
	std::list<SummaryObserver*>::iterator iter = observers.begin();
	while (iter != observers.end())
	{
		SummaryObserver *observer = *iter;
		observer->on_showDetailsClicked(this, id, startTime, stopTime);
		++iter;
	}
}

void Summary::init()
{
}

void Summary::on_selection_changed()
{
	int64_t id = getSelectedID();
	std::list<SummaryObserver*>::iterator iter = observers.begin();
	while (iter != observers.end())
	{
		SummaryObserver *observer = *iter;
		observer->on_selection_changed(id, startTime, stopTime);
		++iter;
	}
}
void Summary::attach(SummaryObserver *observer)
{
	observers.push_back(observer);
}

void Summary::detach(SummaryObserver *observer)
{
	observer->detach(this);
	observers.remove(observer);
}

int64_t Summary::getSelectedID()
{
	int retVal = 0;
	RefPtr<TreeSelection> refTreeSelection = get_selection();
	TreeModel::iterator iter;
	iter = refTreeSelection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row;
		row = *iter;
		retVal = row[columns.col_id];
	}
	return retVal;
}
void Summary::setReferences(Gtk::Calendar &calendar)
{
	this->calendar = &calendar;
	connectSignals();
	on_dateChanged();
}
void Summary::connectSignals()
{
	calendar->signal_day_selected().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
	calendar->signal_day_selected_double_click().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
	calendar->signal_month_changed().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
	calendar->signal_next_month().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
	calendar->signal_next_year().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
	calendar->signal_prev_month().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
	calendar->signal_prev_year().connect(sigc::mem_fun(this, &Summary::on_dateChanged));
}

void Summary::calculateTimeSpan()
{
}

void Summary::on_taskUpdated(int64_t taskID)
{
	if (isVisible())
	{
		shared_ptr<Task> task = taskAccessor->getTask(taskID);
		Gtk::TreeIter iter = findRow(taskID);
		if (iter != treeModel->children().end())
		{
			TreeModel::Row row = *iter;
			time_t totalTime = timeAccessor->getTotalTimeWithChildren(taskID, startTime, stopTime);
			assignValuesToRow(row, task, totalTime);
			int64_t parentID = task->parentID();
			if (parentID > 0)
			{
				on_taskUpdated(parentID);
			}
		}
		else
		{
			empty();
			populate();
		}
	}
	else
	{
		needsRePopulation = true;
	}
}

void Summary::on_taskNameChanged(int64_t id)
{
	on_taskUpdated(id);
}

void Summary::on_taskTimeChanged(int64_t id)
{
	on_taskUpdated(id);
}

void Summary::on_completeUpdate()
{
	if (isVisible())
	{
		empty();
		populate();
	}
	else
	{
		needsRePopulation = true;
	}
}

void Summary::on_taskRemoved(int64_t taskID)
{
	Gtk::TreeIter iter = findRow(taskID);
	if (iter != treeModel->children().end())
	{
		treeModel->erase(iter);
	}
}

void Summary::on_dateChanged()
{
	guint year;
	guint month;
	guint day;
	calendar->get_date(year, month, day);
	activeDay = Utils::getTime(year, month, day);
	time_t old_startTime = startTime;
	time_t old_stopTime = stopTime;
	calculateTimeSpan();
	if (needsRePopulation || old_startTime != startTime || old_stopTime != stopTime)
	{
		if (isVisible())
		{
			empty();
			populate();
		}
		else
		{
			needsRePopulation = true;
		}
	}
}
void Summary::empty()
{
	treeModel->clear();
}

bool Summary::on_focus(Gtk::DirectionType direction)
{
	bool returnValue = Gtk::TreeView::on_focus(direction);
	if (needsRePopulation)
	{
		empty();
		populate();
	}
	return returnValue;
}

bool Summary::isVisible()
{
	Container *parent = this->get_parent();
	Gtk::Notebook *notebook = dynamic_cast<Gtk::Notebook*>(parent->get_parent());
	if (notebook)
	{
		int active_tab = notebook->get_current_page();
		Widget *active = notebook->get_nth_page(active_tab);
		if (active != parent)
		{
			return false;
		}
	}
	return true;
}

TreeModel::Row Summary::add(int64_t id)
{
	std::shared_ptr<Task> task = taskAccessor->getTask(id);

	TreeModel::Row row;
	int64_t parentID = task->parentID();
	if (parentID)
	{
		Gtk::TreeIter iter = findRow(parentID);
		if (iter != treeModel->children().end())
		{
			row = *iter;
		}
		else
		{
			row = add(parentID);
		}
		row = *(treeModel->append((row).children()));
	}
	else
	{
		TreeModel::iterator iter = treeModel->append();
		row = *iter;
	}
	time_t totalTime = timeAccessor->getTotalTimeWithChildren(id, startTime, stopTime);
	assignValuesToRow(row, task, totalTime);
	return row;
}

/*
 * Populate is filling the list, updating existing and adding elements not in list
 */
void Summary::populate(Gtk::TreeModel::Row *parent, int parentID)
{
	if (isVisible())
	{
		std::vector<int64_t> taskIDs = timeAccessor->getActiveTasks(startTime, stopTime);

		for (int64_t id : taskIDs)
		{
			add(id);
		}
		needsRePopulation = false;
	}
	else
	{
		needsRePopulation = true;
	}
}

void Summary::assignValuesToRow(TreeModel::Row &row, std::shared_ptr<Task> task, time_t totalTime)
{
	row[columns.col_id] = task->ID();
	row[columns.col_name] = task->name();
	row[columns.col_time] = Utils::seconds2hhmm(totalTime);
}

Gtk::TreeModel::iterator Summary::findRow(int id)
{
	TreeModel::Children children = treeModel->children();
	return subSearch(id, children);
}

Gtk::TreeModel::iterator Summary::subSearch(int id, TreeModel::Children children)
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

}
