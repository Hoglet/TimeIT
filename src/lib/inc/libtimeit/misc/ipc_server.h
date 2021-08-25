#ifndef IPC_SERVER_H
#define IPC_SERVER_H

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

class ipc_server: public ipc, public timer_observer
{
public:
	ipc_server(string /*socket_name*/, Timer& /*timer*/, notification_manager& /*notifier*/);
	ipc_server(const ipc_server& ) = delete;
	ipc_server(ipc_server&& ) = delete;
	ipc_server& operator=(const ipc_server& ) = delete;
	ipc_server& operator=(ipc_server&& ) = delete;

	~ipc_server() override;
	void poll();
	void on_signal_1_second() override;
private:
	void on_show_menu();
	string socket_name;
	int sock;
	notification_manager& notifier;
};

} /* namespace libtimeit */


#endif /* IPC_SERVER_H */
