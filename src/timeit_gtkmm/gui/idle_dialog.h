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

namespace GUI
{
using namespace libtimeit;

class IIdleDialog
{
public:
	virtual ~IIdleDialog() {};
	virtual void setIdleStartTime(time_t idleStartTime) = 0;
	virtual void setActiveTaskList(std::vector<int64_t> activeTaskIDs) = 0;
	virtual void show() = 0;
	virtual void attach(action_observer*) = 0;
	virtual void detach(action_observer*) = 0;
};

enum IdleDialogResponse{
	RESPONSE_REVERT=1,
	RESPONSE_REVERTANDCONTINUE =2,
	RESPONSE_CONTINUE=3
} ;

class IdleDialog : public Gtk::Dialog, public Timer_observer, public IIdleDialog, public IWidget
{
public:
	IdleDialog(Timer& timer, Database& database);
	virtual void setIdleStartTime(time_t idleStartTime);
	virtual void setActiveTaskList(std::vector<int64_t> activeTaskIDs);
	virtual ~IdleDialog() = default;
	virtual void attach(action_observer*);
	virtual void detach(action_observer*);
	// IWidget interface
	virtual void show();
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	virtual void on_signal_1_second() {};
	virtual void on_signal_10_seconds();
	void setText();
	Gtk::Label  text;
	Gtk::Button revertButton;
	Gtk::Button revertAndContinueButton;
	Gtk::Button continueButton;
	Timer& m_timer;
	time_t m_idleStartTime;
	std::string taskString;
	Task_accessor taskAccessor;

	std::list<action_observer*> observers;
	void responseHandler(int result);
};

}
#endif /* IDLEDIALOG_H_ */
