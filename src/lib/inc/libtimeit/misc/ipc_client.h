#ifndef IPC_CLIENT_H_
#define IPC_CLIENT_H_

#include <string>
#include <memory>
#include <libtimeit/timer.h>
#include <libtimeit/event_observer.h>
#include <iostream>
#include "ipc.h"

namespace libtimeit
{

class ipc_client : public ipc
{
public:
	ipc_client(string socket_name);
	void window_2_front();
private:
	string socket_name;
};

} /* namespace libtimeit */

#endif /* IPC_CLIENT_H_ */
