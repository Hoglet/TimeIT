/*
 * Ipc.h
 *
 *  Created on: 17 Jul 2019
 *      Author: hoglet
 */

#ifndef SRC_MISC_IPC_H_
#define SRC_MISC_IPC_H_

#include <string>

namespace libtimeit
{
using namespace std;

class Ipc
{
public:
	Ipc() = default;
protected:
	static string prepareSocketDir();
};

} /* namespace libtimeit */

#endif /* SRC_MISC_IPC_H_ */
