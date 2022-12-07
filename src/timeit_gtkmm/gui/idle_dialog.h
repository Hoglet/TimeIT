#ifndef IDLE_DIALOG_H
#define IDLE_DIALOG_H

#include <gtkmm.h>
#include <libtimeit/timer.h>
#include <widget_interface.h>
#include <memory>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/logic/time_keeper.h>

namespace gui
{
using namespace libtimeit;


enum idle_dialog_response{
	RESPONSE_REVERT=1,
	RESPONSE_REVERTANDCONTINUE =2,
	RESPONSE_CONTINUE=3
} ;

class idle_dialog : public Gtk::Dialog, public timer_observer, public widget_interface
{
public:
	idle_dialog( timer_base& timer, database& db, time_manager& op_time_keeper);
	void set_time_id(const time_id& id);
	// IWidget interface
	void show() override;
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); } ;
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position( int& x, int& y) override { Gtk::Dialog::get_position( x, y); };
private:
	void on_signal_10_seconds() override;
	void set_text();
	Gtk::Label    text;
	Gtk::Button   revert_button;
	Gtk::Button   revert_and_continue_button;
	Gtk::Button   continue_button;
	string        task_string;
	task_accessor tasks;
	time_accessor times;
	time_manager&  time_keeper;

	time_id       time_entry_id;

	time_point<system_clock>  idle_start_time { system_clock::from_time_t(0) };

	void response_handler(int result);
	void action_continue(const time_id& id);
	void revert_and_stop(const time_id& id);
	void revert_and_continue(const time_id& id);
};

}
#endif /* IDLE_DIALOG_H */
