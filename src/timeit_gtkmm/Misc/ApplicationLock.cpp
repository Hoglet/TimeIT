/*
 * ApplicationLock.cpp
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#include "ApplicationLock.h"
#include <string>
#include <unistd.h>
#include <fcntl.h>

ApplicationLock::ApplicationLock(const std::string& lockBase)
{
	locked = false;
	std::string fileName = lockBase + ".lock";
	//
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	if ((fdlock = open(fileName.c_str(), O_WRONLY|O_CREAT, 0666)) != -1)
	{
		if (fcntl(fdlock, F_SETLK, &fl) != -1)
		{
			locked = true;
		}
	}
}
bool ApplicationLock::lockAquired()
{
	return locked;
}


ApplicationLock::~ApplicationLock()
{
	if(fdlock != -1)
	{
		close(fdlock);
	}
}
