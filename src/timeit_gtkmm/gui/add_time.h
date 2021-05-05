/*
 * AddTime.h
 *
 *  Created on: 2008-okt-26
 *      Author: hoglet
 */

#ifndef ADDTIME_H_
#define ADDTIME_H_

#include <gtkmm.h>
#include "lz_spin_button.h"
#include "ICalendar.h"
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include <IWidget.h>
namespace gui
{
using namespace libtimeit;

class AddTime: public Gtk::Dialog, public CalendarObserver, public IWidget
{
public:
	AddTime(int64_t taskId, ICalendar& calendar, Database& database);
	AddTime(const AddTime&)             = delete;
	AddTime( AddTime&&)                 = delete;
	AddTime& operator=(const AddTime& ) = delete;
	AddTime& operator=( AddTime&& )     = delete;
	virtual ~AddTime();
	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void on_date_changed() override;
	void on_month_changed();
	void on_change();
	void on_response(int response_id) override;
	Gtk::Table table;
	Gtk::HBox  hbox;
	Gtk::HSeparator hseparator;

	Gtk::Label yearLabel;
	Gtk::Label monthLabel;
	Gtk::Label dayLabel;
	Gtk::Label taskNameLabel;
	Gtk::Label taskName;

	Gtk::Label startTimeLabel;
	Gtk::Label stopTimeLabel;
	Gtk::Label startColonLabel, toLabel, stopColonLabel;
	LZSpinButton startTimeHour;
	LZSpinButton startTimeMinute;
	LZSpinButton stopTimeHour;
	LZSpinButton stopTimeMinute;
	LZSpinButton year;
	LZSpinButton month;
	LZSpinButton day;
//	Gtk::Button CancelButton;
	Gtk::Button* OKButton;
	ICalendar& calendar;

	int64_t taskID;
	Time_accessor m_timeAccessor;
	Task_accessor m_taskAccessor;
};
}

#endif /* ADDTIME_H_ */
