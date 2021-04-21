#include "summary.h"
#include "libtimeit/utils.h"
#include "details_dialog.h"
#include <glibmm/i18n.h>
using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace libtimeit;

namespace GUI
{
using namespace std;
using namespace libtimeit;

SummaryObserver::SummaryObserver()
{
	unsubscription_allowed = true;
}
SummaryObserver::~SummaryObserver()
{
	unsubscription_allowed = false;
	list<ISummary*>::iterator iter = subjects.begin();
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

Summary::Summary(
		Database &database,
		Notifier& notifier)
		:
		Event_observer(notifier),
		timeAccessor(database),
		taskAccessor(database)
{
	treeModel = TreeStore::create(columns);
	set_model(treeModel);
	append_column("Name", columns.col_name);
	append_column("Time", columns.col_time);
	set_headers_visible(false);

	refTreeSelection = get_selection();
	refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &Summary::on_selection_changed));

	Gtk::Menu::MenuList &menulist = Menu_Popup.items();

	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Show details"), sigc::mem_fun(*this, &Summary::on_menu_showDetails)));

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
		observer->on_showDetailsClicked( id, startTime, stopTime);
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

void Summary::on_task_updated(int64_t taskID)
{
	if (isVisible())
	{
		auto task = taskAccessor.by_ID(taskID);
		Gtk::TreeIter iter = findRow(taskID);
		if (task.has_value() && iter != treeModel->children().end())
		{
			TreeModel::Row row = *iter;
			time_t totalTime = timeAccessor.total_cumulative_time(taskID, startTime, stopTime);
			assignValuesToRow(row, *task, totalTime);
			int64_t parentID = task->parent_ID;
			if (parentID > 0)
			{
				on_task_updated(parentID);
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

void Summary::on_task_name_changed(int64_t id)
{
	on_task_updated(id);
}

void Summary::on_task_time_changed(int64_t id)
{
	on_task_updated(id);
}

void Summary::on_complete_update()
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

void Summary::on_task_removed(int64_t taskID)
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
	activeDay = libtimeit::to_time(year, month, day);
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
	auto task = taskAccessor.by_ID(id);
	TreeModel::Row row;
	int64_t parentID = task->parent_ID;
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
	time_t totalTime = timeAccessor.total_cumulative_time(id, startTime, stopTime);
	assignValuesToRow(row, *task, totalTime);
	return row;
}

void Summary::populate()
{
	if (isVisible())
	{
		std::vector<int64_t> taskIDs = timeAccessor.active_tasks(startTime, stopTime);

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

void Summary::assignValuesToRow(TreeModel::Row &row, Task& task, time_t totalTime)
{
	row[columns.col_id] = task.ID;
	row[columns.col_name] = task.name;
	row[columns.col_time] = libtimeit::seconds_2_hhmm(totalTime);
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
