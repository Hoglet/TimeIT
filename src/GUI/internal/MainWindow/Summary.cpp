/*
 * DaySummary.cpp
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#include "Summary.h"
#include "Utils.h"
#include "DetailsDialog.h"

using namespace std;
using namespace Gtk;
using namespace Glib;

namespace GUI
{
namespace Internal
{

Summary::Summary(boost::shared_ptr<DB::Database>& database)
{
	timeAccessor = database->getTimeAccessor();
	taskAccessor = database->getTaskAccessor();
	treeModel = TreeStore::create(columns);
	set_model(treeModel);
	append_column("Name", columns.col_name);
	append_column("Time", columns.col_time);
	set_headers_visible(false);

	refTreeSelection = get_selection();
	refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &Summary::on_selection_changed));
	taskAccessor->attach(this);
}

Summary::~Summary()
{
	taskAccessor->detach(this);
}

bool Summary::on_button_press_event(GdkEventButton* event)
{
	bool retVal = Gtk::TreeView::on_button_press_event(event);
	if (event->type == GDK_2BUTTON_PRESS)
	{
		int64_t id = getSelectedID();
		std::list<SummaryObserver*>::iterator iter = observers.begin();
		while (iter != observers.end())
		{
			SummaryObserver* observer = *iter;
			observer->on_showDetailsClicked(this, id, startTime, stopTime);
			iter++;
		}
	}
	return retVal;
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
		SummaryObserver* observer = *iter;
		observer->on_selection_changed(id, startTime, stopTime);
		iter++;
	}
}
void Summary::attach(SummaryObserver* observer)
{
	observers.push_back(observer);
}

void Summary::detach(SummaryObserver* observer)
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
		//Do something with the row.
	}
	return retVal;
}
void Summary::setReferences(Gtk::Calendar& calendar)
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

Gtk::TreeModel::iterator Summary::findRow(int id)
{
	TreeModel::Children children = treeModel->children();
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		if (((*iter)[columns.col_id] == id))
		{
			break;
		}
	}
	return iter;
}

void Summary::calculateTimeSpan()
{
}

void Summary::on_taskUpdated(const Task&)
{
	populate();
}

void Summary::on_taskRemoved(int64_t)
{
	empty();
	populate();
}

void Summary::on_dateChanged()
{
	guint year;
	guint month;
	guint day;
	calendar->get_date(year, month, day);
	activeDay = Utils::getTime(year, month, day);
	calculateTimeSpan();
	empty();
	populate();
}
void Summary::empty()
{
	treeModel->clear();
}

/*
 * Populate is filling the list, updating existing and adding elements not in list
 */
void Summary::populate(Gtk::TreeModel::Row* parent, int parentID)
{
	//	std::map<int64_t, TaskTime> timeList = timeAccessor->getTimeList(startTime, stopTime);
	vector<Task> tasks = taskAccessor->getTasks(parentID, startTime, stopTime);

	for (int i = 0; i < (int) tasks.size(); i++)
	{
		Task task = tasks.at(i);
		if (task.getTotalTime() > 0)
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
			//assignValuesToRow
			row[columns.col_id] = task.getID();
			row[columns.col_name] = task.getName();
			row[columns.col_time] = Utils::seconds2hhmm(task.getTotalTime());
			//
			populate(&row, task.getID());
			TreeModel::Path path(iter);
			this->expand_row(path, false);
		}
	}
	//for (int i=0; i < (int)tasks.size(); i++)
	/*std::map<int64_t, TaskTime>::iterator iter;
	 for (iter = timeList.begin(); iter != timeList.end(); iter++)
	 {
	 int64_t id = iter->first;
	 TaskTime taskTime = iter->second;

	 Gtk::TreeIter treeIter = findRow(id);
	 if (treeIter == treeModel->children().end())
	 {
	 treeIter = treeModel->append();
	 }
	 TreeModel::Row row;
	 row = *treeIter;
	 row[columns.col_id] = id;
	 row[columns.col_name] = taskTime.name;
	 row[columns.col_time] = Utils::seconds2hhmm(taskTime.duration);
	 }*/
}

}
}
