/*
 * IdleDialog.h
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#ifndef IDLEDIALOG_H_
#define IDLEDIALOG_H_

#include <gtkmm.h>
#include <Timer.h>
#include <IIdleDialog.h>
#include <boost/shared_ptr.hpp>
#include <IWidget.h>
namespace GUI
{
namespace Internal
{

enum IdleDialogResponse{
	RESPONSE_REVERT=1,
	RESPONSE_REVERTANDCONTINUE =2,
	RESPONSE_CONTINUE=3
} ;

class IdleDialog : public Gtk::Dialog, public TimerObserver, public IIdleDialog, public IWidget
{
public:
	IdleDialog(boost::shared_ptr<Timer>& timer);
	void setIdleStartTime(time_t idleStartTime);
	virtual ~IdleDialog();
	virtual void attach(IActionObserver*);
	virtual void detach(IActionObserver*);
	// IWidget interface
	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { return Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	virtual void on_signal_1_second() {};
	virtual void on_signal_10_seconds();
	void setText();
	void updater();
	void run();
	Gtk::Label  text;
	Gtk::Button revertButton;
	Gtk::Button revertAndContinueButton;
	Gtk::Button continueButton;
	boost::shared_ptr<Timer> m_timer;
	time_t m_idleStartTime;

	std::list<IActionObserver*> observers;
	void responseHandler(int result);
};

}
}
#endif /* IDLEDIALOG_H_ */
