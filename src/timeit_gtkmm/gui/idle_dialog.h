/*
 * IdleDialog.h
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#ifndef IDLEDIALOG_H_
#define IDLEDIALOG_H_

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

class IdleDialog : public Gtk::Dialog, public Timer_observer, public IWidget
{
public:
	IdleDialog(Timer& timer, Database& database, Time_keeper& time_keeper);
	void set_time_id(Time_id id);
	// IWidget interface
	void show();
	void hide() { Gtk::Dialog::hide(); }
	void move(int x, int y) { Gtk::Dialog::move(x,y); };
	bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void on_signal_10_seconds() override;
	void setText();
	Gtk::Label    text;
	Gtk::Button   revertButton;
	Gtk::Button   revertAndContinueButton;
	Gtk::Button   continueButton;
	string        taskString;
	Task_accessor taskAccessor;
	Time_accessor time_accessor;
	Time_keeper&  time_keeper;

	Time_id       time_entry_id;
	time_t        m_idleStartTime;

	void responseHandler(int result);
	void action_continue(Time_id id);
	void revert_and_stop(Time_id id);
	void revert_and_continue(Time_id id);
};

}
#endif /* IDLEDIALOG_H_ */
