/*
 * AddTime.h
 *
 *  Created on: 2008-okt-26
 *      Author: hoglet
 */

#ifndef ADDTIME_H_
#define ADDTIME_H_

#include <gtkmm.h>
#include "LZSpinButton.h"
#include "Calendar.h"
#include <Database.h>
#include <IWidget.h>
namespace GUI {
namespace Internal {
class AddTime: public Gtk::Dialog, public CalendarObserver, public IWidget
{
public:
	AddTime(int64_t taskId, ICalendar& calendar, boost::shared_ptr<DB::Database>& database);
	virtual ~AddTime();
	// IWidget interface
	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { return Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	virtual void on_date_changed();
	void on_month_changed();
	void on_change();
	virtual void on_response(int response_id);
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
	Gtk::ScrolledWindow scrolledWindow;
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
	boost::shared_ptr<ITimeAccessor> m_timeAccessor;
	boost::shared_ptr<ITaskAccessor> m_taskAccessor;
};
}
}

#endif /* ADDTIME_H_ */
