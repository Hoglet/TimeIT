/*
 * Ipc.h
 *
 *  Created on: 11 Jul 2019
 *      Author: hoglet
 */

#ifndef SRC_MISC_IPCSERVER_H_
#define SRC_MISC_IPCSERVER_H_

#include <string>
#include <memory>
#include <libtimeit/timer.h>
#include <libtimeit/event_observer.h>
#include <iostream>
#include "libtimeit/misc/ipc.h"

namespace libtimeit
{
using namespace std;
using namespace libtimeit;

class IpcServer: public Ipc, public Timer_observer
{
public:
	IpcServer(string socketName, Timer&);
	virtual ~IpcServer();
	void poll();
	virtual void on_signal_1_second();
	void attach(Event_observer*);
	void detach(Event_observer*);
	private:
	void on_show_menu();
	string socketName;
	list<Event_observer*> observers;
	int sock;
};

} /* namespace libtimeit */


#endif /* SRC_MISC_IPCSERVER_H_ */
