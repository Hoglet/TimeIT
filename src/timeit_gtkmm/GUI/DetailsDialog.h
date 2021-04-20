/*
 * EditTimeEntry.h
 *
 *  Created on: 2008-aug-21
 *      Author: hoglet
 */

#ifndef DETAILSDIALOG_H_
#define DETAILSDIALOG_H_

#include <gtkmm.h>
#include "LZSpinButton.h"
#include "Details.h"
#include "MainWindow/Summary.h"
#include <libtimeit/db/database.h>
#include <IWidget.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>

namespace GUI
{
using namespace libtimeit;


class DetailsDialog:
		public Gtk::Dialog,
		public SummaryObserver,
		public DetailsObserver,
		public Event_observer,
		public IWidget
{
public:
	static std::shared_ptr<DetailsDialog> create(Database& database, Time_keeper &timeKeeper, Notifier& notifier);
	virtual ~DetailsDialog() ;
	void setTimeEntryID(int64_t id);
	void set(int64_t ID,time_t startTime,time_t stopTime);


	// IWidget interface
	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };

	void on_runningTasksChanged();
	void on_task_name_updated(int64_t task_id);
	void on_task_total_time_updated(int64_t task_id);

private:
	DetailsDialog(Database& database, Time_keeper &timeKeeper, Notifier& notifier);

	//SummaryObserver
	virtual void on_selection_changed(int64_t ID,time_t startTime,time_t stopTime);
	virtual void on_showDetailsClicked(int64_t taskId, time_t startTime, time_t stopTime) {};

	//DetailsObserver
	void on_selected_changed();
	void on_edit_details(int64_t timeEntryID);

	//EventObserver
	virtual void on_task_time_changed(int64_t task_ID);
	virtual void on_task_name_changed(int64_t task_ID);

	void setValues();
	void on_OKButton_clicked();
	void on_CancelButton_clicked();
	void on_change();
	void checkForChanges();

	Gtk::Table table1;
	Gtk::Label taskName;
	Gtk::Image runningImage;
	Details detailList;
	Gtk::Label taskTotalTime;
	Glib::RefPtr<Gdk::Pixbuf> runningIcon;
	Glib::RefPtr<Gdk::Pixbuf> runningIdleIcon;
	Glib::RefPtr<Gdk::Pixbuf> blankIcon;
	Gtk::Table table2;
	Gtk::Label startTimeLabel;
	Gtk::Label stopTimeLabel;
	Gtk::Label startColonLabel, toLabel, stopColonLabel;
	Gtk::ScrolledWindow scrolledWindow;
	LZSpinButton startTimeHour;
	LZSpinButton startTimeMinute;
	LZSpinButton stopTimeHour;
	LZSpinButton stopTimeMinute;
	Gtk::Button CancelButton;
	Gtk::Button OKButton;
//	static DetailsDialog* instance;
	time_t  oldStartTime;
	time_t  oldStopTime;
	time_t  startTime;
	time_t  stopTime;
	time_t  rangeStart;
	time_t  rangeStop;
	int64_t m_taskID;
	int64_t m_timeEntryID;
	Time_accessor          m_timeAccessor;
	Extended_task_accessor m_taskAccessor;
	Settings_accessor      settings_accessor;
	Time_keeper&           m_timeKeeper;
};
}

#endif /* DETAILSDIALOG_H_ */
