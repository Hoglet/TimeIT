/*
 * ApplicationLock.cpp
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#include "libtimeit/misc/application_lock.h"
#include <string>
#include <unistd.h>
#include <fcntl.h>
namespace libtimeit
{


Application_lock::Application_lock(string lock_base)
{
	locked = false;
	string fileName = lock_base + ".lock";
	//
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	if ((fd_lock = open(fileName.c_str(), O_WRONLY | O_CREAT, 0666)) != -1)
	{
		if (fcntl(fd_lock, F_SETLK, &fl) != -1)
		{
			locked = true;
		}
	}
}

bool Application_lock::lock_acquired() const
{
	return locked;
}


Application_lock::~Application_lock()
{
	if (fd_lock != -1)
	{
		close(fd_lock);
	}
}

}