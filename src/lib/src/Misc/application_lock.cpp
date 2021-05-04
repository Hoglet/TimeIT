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


static const int RW_RW_RW = 0666;

Application_lock::Application_lock(string lock_base) : locked(false)
{

	string file_name = lock_base + ".lock";
	//
	struct flock fl{};

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	if ((fd_lock = open(file_name.c_str(), O_WRONLY | O_CREAT, RW_RW_RW)) != -1) // NOLINT
	{
		if (fcntl(fd_lock, F_SETLK, &fl) != -1) // NOLINT
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