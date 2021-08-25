#ifndef IDLE_DIALOG_H
#define IDLE_DIALOG_H

#include <gtkmm.h>
#include <libtimeit/timer.h>
#include <IWidget.h>
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

class idle_dialog : public Gtk::Dialog, public timer_observer, public IWidget
{
public:
	idle_dialog(Timer& timer, database& db, Time_keeper& op_time_keeper);
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
	Time_keeper&  time_keeper;

	time_id       time_entry_id;
	time_t        idle_start_time{0};

	void response_handler(int result);
	void action_continue(const time_id& id);
	void revert_and_stop(const time_id& id);
	void revert_and_continue(const time_id& id);
};

}
#endif /* IDLE_DIALOG_H */
