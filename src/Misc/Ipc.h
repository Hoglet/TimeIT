/*
 * Ipc.h
 *
 *  Created on: 17 Jul 2019
 *      Author: hoglet
 */

#ifndef SRC_MISC_IPC_H_
#define SRC_MISC_IPC_H_

#include <string>

namespace Utils
{

class Ipc
{
public:
	Ipc();
	virtual ~Ipc();
protected:
	std::string prepareSocketDir();
};

} /* namespace Utils */

#endif /* SRC_MISC_IPC_H_ */
