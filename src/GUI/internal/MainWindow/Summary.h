/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARY_H_
#define SUMMARY_H_

#include <gtkmm.h>
#include <ISummary.h>
#include <ITimeAccessor.h>
#include <Database.h>
#include <ITaskAccessor.h>

namespace GUI
{
namespace Internal
{

class Summary: public Gtk::TreeView, public TaskAccessorObserver, public ISummary
{
public:
	Summary(std::shared_ptr<DB::Database>& database);
	virtual ~Summary();
	void setReferences(Gtk::Calendar& calendar);
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
protected:
	Gtk::Menu Menu_Popup;

	bool on_button_press_event(GdkEventButton* event);

	//SummaryObserver
	virtual void on_selection_changed();
	virtual void on_showDetailsClicked(ISummary* summary,int64_t taskId, time_t startTime, time_t stopTime) {};

	//
	void connectSignals();
	void on_dateChanged();
	virtual void on_taskAdded(int64_t)
	{
	};
	void on_menu_showDetails();
	virtual void on_taskUpdated(int64_t);
	virtual void on_taskRemoved(int64_t);
	void init();
	void populate(Gtk::TreeModel::Row* parent=0, int parentID=0);
	void empty();
	virtual void calculateTimeSpan();
	Gtk::TreeModel::iterator findRow(int id);
	Gtk::TreeModel::iterator subSearch(int id, Gtk::TreeModel::Children children);
	void assignValuesToRow(Gtk::TreeModel::Row& row, const Task& task);

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
	Gtk::Calendar* calendar;
	time_t activeDay;
	time_t startTime;
	time_t stopTime;
	std::list<SummaryObserver* > observers;
	std::shared_ptr<ITimeAccessor> timeAccessor;
	std::shared_ptr<ITaskAccessor> taskAccessor;
};

}
}
#endif /* DAYSUMMARY_H_ */
