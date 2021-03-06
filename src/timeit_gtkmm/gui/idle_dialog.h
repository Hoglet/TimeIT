#ifndef IDLE_DIALOG_H_
#define IDLE_DIALOG_H_

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


enum IdleDialogResponse{
	RESPONSE_REVERT=1,
	RESPONSE_REVERTANDCONTINUE =2,
	RESPONSE_CONTINUE=3
} ;

class Idle_dialog : public Gtk::Dialog, public Timer_observer, public IWidget
{
public:
	Idle_dialog(Timer& timer, Database& database, Time_keeper& time_keeper);
	void set_time_id(Time_id id);
	// IWidget interface
	void show() override;
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); } ;
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void on_signal_10_seconds() override;
	void set_text();
	Gtk::Label    text;
	Gtk::Button   revert_button;
	Gtk::Button   revert_and_continue_button;
	Gtk::Button   continue_button;
	string        task_string;
	Task_accessor task_accessor;
	Time_accessor time_accessor;
	Time_keeper&  time_keeper;

	Time_id       time_entry_id{0};
	time_t        idle_start_time{0};

	void response_handler(int result);
	void action_continue(Time_id id);
	void revert_and_stop(Time_id id);
	void revert_and_continue(Time_id id);
};

}
#endif /* IDLE_DIALOG_H_ */
