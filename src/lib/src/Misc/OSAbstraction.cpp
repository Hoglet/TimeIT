/*
 * OSabstraction.cpp
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#include "Misc/OSAbstraction.h"
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

namespace OSAbstraction
{

//LCOV_EXCL_START
void mkDir(std::string path)
{
	stringstream mkdirCommand;
	mkdirCommand << "mkdir -p \"" << path << "\"";
	system(mkdirCommand.str().c_str());
}
//LCOV_EXCL_STOP

void unlink(std::string filename)
{
	::unlink(filename.c_str());
}

// Code converted from http://www.techbytes.ca/techbyte103.html
bool fileExists(std::string filename)
{
	struct stat stFileInfo;
	bool retVal;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(filename.c_str(), &stFileInfo);
	if (intStat == 0)
	{
		// We were able to get the file attributes
		// so the file obviously exists.
		retVal = true;
	}
	else
	{
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		retVal = false;
	}

	return retVal;
}

//LCOV_EXCL_START

void showURL(std::string url)
{
	std::stringstream command;
	command << "xdg-open " << url << endl;
	system(command.str().c_str());
}
//LCOV_EXCL_STOP
}
