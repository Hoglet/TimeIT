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
#include <libtimeit/Timer.h>
#include <libtimeit/Event_observer.h>
#include <iostream>
#include "libtimeit/misc/Ipc.h"

namespace libtimeit
{
using namespace std;
using namespace libtimeit;

class IpcServer: public Ipc, public TimerObserver
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
	Timer& timer;
	list<Event_observer*> observers;
	int sock;
};

} /* namespace libtimeit */


#endif /* SRC_MISC_IPCSERVER_H_ */
