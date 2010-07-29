/*
 * IdleDialog.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include "IdleDialog.h"
#include <iostream>
#include <string>
#include <sstream>
#include <glibmm/i18n.h>
#include "Timer.h"

namespace GUI
{
namespace Internal
{

IdleDialog::IdleDialog(boost::shared_ptr<Timer>& timer)
{
	m_timer = timer;

	//Setting start tim to now in case nobody will set the idle time later.
	m_idleStartTime = time(NULL);

	set_deletable(false);
	setText();
	get_vbox()->pack_start(text);
	add_button(_("Revert and stop"), RESPONSE_REVERT);
	add_button(_("Revert and continue"), RESPONSE_REVERTANDCONTINUE);
	add_button(_("Continue"), RESPONSE_CONTINUE);

	m_timer->attach(this);

	signal_response().connect(sigc::mem_fun(this, &IdleDialog::responseHandler));

	//set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);
	show_all_children();

}

IdleDialog::~IdleDialog()
{
	m_timer->detach(this);
}

void IdleDialog::attach(IActionObserver* observer)
{
	observers.push_back(observer);
}
void IdleDialog::detach(IActionObserver* observer)
{
	observers.remove(observer);
}

void IdleDialog::setIdleStartTime(time_t idleStartTime)
{
	m_idleStartTime = idleStartTime;
}

void IdleDialog::on_signal_10_seconds()
{
	setText();
}

void IdleDialog::setText()
{
	std::stringstream str;
	int minutesIdle = (time(NULL) - m_idleStartTime) / 60;
	// Note to translators: This is the first part of the sentence "No activity have been detected
	// for X minutes. What should we do?"
	str << _("No activity have been detected for") << " "<<minutesIdle;
	// Note to translators: This is the second part of the sentence "No activity have been detected
	// for X minutes. What should we do?"
	str<< " "<<_("minutes. What should we do?");
	text.set_text(str.str());
}

void IdleDialog::responseHandler(int result)
{
	std::list<IActionObserver*>::iterator iter;
	std::list<IActionObserver*> observers = this->observers;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		switch (result)
		{
		case (RESPONSE_REVERT):
			observer->on_action_revertAndStop();
			break;
		case (RESPONSE_REVERTANDCONTINUE):
			observer->on_action_revertAndContinue();
			break;
		default:
			std::cout << "Unexpected button clicked." << std::endl;
			//Fall trough
		case (RESPONSE_CONTINUE):
			observer->on_action_continue();
			break;
		}
	}
	hide();
}

}
}
