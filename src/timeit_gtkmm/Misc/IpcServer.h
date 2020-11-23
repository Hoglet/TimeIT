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
#include <IActionObserver.h>
#include <iostream>
#include "Ipc.h"

namespace Utils
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
	void attach(IActionObserver*);
	void detach(IActionObserver*);
	private:
	void on_show_menu();
	string socketName;
	Timer& timer;
	list<IActionObserver*> observers;
	int sock;
};

} /* namespace Utils */


#endif /* SRC_MISC_IPCSERVER_H_ */
