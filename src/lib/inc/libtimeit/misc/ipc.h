#ifndef IPC_H
#define IPC_H

#include <string>

namespace libtimeit
{
using namespace std;

class ipc
{
public:
	ipc() = default;
protected:
	static string prepare_socket_dir();
};

} /* namespace libtimeit */

#endif /* IPC_H */
