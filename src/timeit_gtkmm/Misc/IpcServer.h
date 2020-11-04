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
#include <Timer.h>
#include <IActionObserver.h>
#include <iostream>
#include "Ipc.h"

namespace Utils
{

class IpcServer: public Ipc, public TimerObserver
{
public:
	IpcServer(std::string socketName, std::shared_ptr<Timer>);
	virtual ~IpcServer();
	void poll();
	virtual void on_signal_1_second();
	void attach(IActionObserver*);
	void detach(IActionObserver*);
	private:
	void on_show_menu();
	std::string socketName;
	std::shared_ptr<Timer> timer;
	std::list<IActionObserver*> observers;
	int sock;
};

} /* namespace Utils */

#endif /* SRC_MISC_IPCSERVER_H_ */
