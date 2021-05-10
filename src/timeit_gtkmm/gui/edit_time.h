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
#include <main_window/calendar.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include <IWidget.h>
namespace gui
{
using namespace libtimeit;

/*
 * Edit and add times
 */
class Edit_time: public Gtk::Dialog, public IWidget
{
public:
	Edit_time(Time_entry time_entry, Database& database);
	Edit_time(const Edit_time&)             = delete;
	Edit_time(Edit_time&&)                 = delete;
	Edit_time& operator=(const Edit_time& ) = delete;
	Edit_time& operator=(Edit_time&& )     = delete;
	~Edit_time() = default;

	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void connect_signals(vector<Gtk::SpinButton*> widgets);
	void set_values( );
	void on_change();
	void on_response(int response_id) override;

	Gtk::Table table;
	Gtk::HBox  hbox;
	Gtk::HSeparator hseparator;

	Gtk::Label year_label;
	Gtk::Label month_label;
	Gtk::Label day_label;
	Gtk::Label task_name_label;
	Gtk::Label task_name;

	Gtk::Label start_time_label;
	Gtk::Label stop_time_label;
	Gtk::Label start_colon_label;
	Gtk::Label to_label;
	Gtk::Label stop_colon_label;

	LZSpinButton start_time_hour;
	LZSpinButton start_time_minute;
	LZSpinButton stop_time_hour;
	LZSpinButton stop_time_minute;
	LZSpinButton year;
	LZSpinButton month;
	LZSpinButton day;
	Gtk::Button* ok_button;

	Time_entry    time_entry;
	Time_accessor time_accessor;
	Task_accessor task_accessor;
};
}

#endif /* ADDTIME_H_ */
