/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARY_H_
#define SUMMARY_H_

#include <gtkmm.h>
#include <libtimeit/db/TimeAccessor.h>
#include <libtimeit/db/Database.h>
#include <libtimeit/db/TaskAccessor.h>

namespace GUI
{

using namespace libtimeit;
using namespace std;

class ISummary;
class SummaryObserver
{
public:
	SummaryObserver();
	virtual ~SummaryObserver();
	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime) = 0;
	virtual void on_showDetailsClicked(ISummary *summary, int64_t taskId, time_t startTime, time_t stopTime) = 0;
	void attach(ISummary *subject);
	void detach(ISummary *subject);
private:
	bool unsubscription_allowed;
	std::list<ISummary*> subjects;
};
class ISummary
{
public:
	virtual ~ISummary()
	{
	}
	;
	virtual void attach(SummaryObserver*) = 0;
	virtual void detach(SummaryObserver*) = 0;
};

class Summary: public Gtk::TreeView, public EventObserver, public ISummary
{
public:
	Summary(IDatabase &database);
	virtual ~Summary();
	void setReferences(Gtk::Calendar &calendar);
	int64_t getSelectedID();
	time_t getStartTime()
	{
		return startTime;
	}
	time_t getStopTime()
	{
		return stopTime;
	}
	void attach(SummaryObserver*);
	void detach(SummaryObserver*);
	virtual bool on_focus(Gtk::DirectionType direction);
protected:
	Gtk::TreeModel::Row add(int64_t id);
	Gtk::Menu Menu_Popup;

	bool on_button_press_event(GdkEventButton *event);

	//SummaryObserver
	virtual void on_selection_changed();
	virtual void on_showDetailsClicked(ISummary *summary, int64_t taskId, time_t startTime, time_t stopTime)
	{
	}
	;

	//
	void connectSignals();
	void on_dateChanged();
	virtual void on_taskAdded(int64_t)
	{
	}
	;
	void on_menu_showDetails();
	virtual void on_taskUpdated(int64_t);
	virtual void on_taskRemoved(int64_t);
	virtual void on_completeUpdate();
	virtual void on_taskNameChanged(int64_t);
	virtual void on_taskTimeChanged(int64_t);
	void init();
	void populate(Gtk::TreeModel::Row *parent = 0, int parentID = 0);
	void empty();
	virtual void calculateTimeSpan();
	Gtk::TreeModel::iterator findRow(int id);
	Gtk::TreeModel::iterator subSearch(int id, Gtk::TreeModel::Children children);
	void assignValuesToRow(Gtk::TreeModel::Row &row, std::shared_ptr<Task> task, time_t totalTime);

	Glib::RefPtr<Gtk::TreeStore> treeModel;
	class ModelColumns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{

			add(col_id);
			add(col_name);
			add(col_time);
		}
		;
		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
	};
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;
	ModelColumns columns;
	Gtk::Calendar *calendar = nullptr;
	time_t activeDay = 0;
	time_t startTime = 0;
	time_t stopTime = 0;
	std::list<SummaryObserver*> observers;
	std::shared_ptr<ITimeAccessor> timeAccessor;
	std::shared_ptr<ITaskAccessor> taskAccessor;
private:
	bool isVisible();
	bool needsRePopulation = true;
};

}
#endif /* DAYSUMMARY_H_ */
