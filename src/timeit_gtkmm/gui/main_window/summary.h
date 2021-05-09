/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARY_H_
#define SUMMARY_H_

#include <gtkmm.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>

namespace gui
{

using namespace libtimeit;
using namespace std;

class ISummary;
class SummaryObserver
{
public:
	SummaryObserver();
	SummaryObserver( const SummaryObserver& )            = delete;
	SummaryObserver( SummaryObserver&& )                 = delete;
	SummaryObserver& operator=( const SummaryObserver& ) = delete;
	SummaryObserver& operator=( SummaryObserver&& )      = delete;
	virtual ~SummaryObserver();
	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime) = 0;
	virtual void on_show_details_clicked(int64_t taskId, time_t startTime, time_t stopTime) = 0;
	void attach(ISummary *subject);
	void detach(ISummary *subject);
private:
	bool unsubscription_allowed{true};
	std::list<ISummary*> subjects;
};
class ISummary
{
public:
	virtual void attach(SummaryObserver*) = 0;
	virtual void detach(SummaryObserver*) = 0;
};

class Summary: public Gtk::TreeView, public Event_observer, public ISummary
{
public:
	Summary(Database &database, Notifier& notifier);
	void setReferences(Gtk::Calendar &calendar);
	int64_t getSelectedID();
	time_t getStartTime() const
	{
		return startTime;
	}
	time_t getStopTime() const
	{
		return stopTime;
	}
	void attach(SummaryObserver* /*observer*/);
	void detach(SummaryObserver* /*observer*/);
	virtual bool on_focus(Gtk::DirectionType direction);
protected:
	Gtk::TreeModel::Row add(int64_t id);
	Gtk::Menu Menu_Popup;

	bool on_button_press_event(GdkEventButton *event);

	//SummaryObserver
	virtual void on_selection_changed();

	//
	void connectSignals();
	void on_dateChanged();
	virtual void on_task_added(int64_t)
	{
	}
	;
	void on_menu_showDetails();
	virtual void on_task_updated(int64_t);
	virtual void on_task_removed(int64_t);
	virtual void on_complete_update();
	virtual void on_task_name_changed(int64_t);
	virtual void on_task_time_changed(int64_t);
	void init();
	void populate();
	void empty();
	virtual void calculateTimeSpan();
	Gtk::TreeModel::iterator findRow(Task_id id);
	Gtk::TreeModel::iterator subSearch(int id, Gtk::TreeModel::Children children);
	void assignValuesToRow(Gtk::TreeModel::Row &row, Task& task, time_t totalTime);

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
	Time_accessor timeAccessor;
	Task_accessor taskAccessor;
private:
	bool isVisible();
	bool needsRePopulation = true;
};

}
#endif /* DAYSUMMARY_H_ */
