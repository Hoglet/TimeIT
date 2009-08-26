/*
 * OSabstraction.cpp
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#include "OSAbstraction.h"
#include <gtkmm.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <strstream>

using namespace std;

void OSAbstraction::mkDir(std::string path)
{
	stringstream mkdirCommand;
	mkdirCommand<<"mkdir -p \""<<path<<"\"";
	system(mkdirCommand.str().c_str());
}
void OSAbstraction::unlink(std::string filename)
{
	::unlink(filename.c_str());
}


// Code converted from http://www.techbytes.ca/techbyte103.html
bool OSAbstraction::fileExists(std::string filename)
{
  struct stat stFileInfo;
  bool retVal;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(filename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
	  retVal = true;
  } else {
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


//
//
// Code converted from http://www.flipcode.com/archives/Path_To_Executable_On_Linux.shtml
std::string OSAbstraction::getApplicationPath()
{
	const int size=500;
	char buff[size+1];
	std::string retVal;
	std::stringstream linkname;

	/* Get our PID and build the name of the link in /proc */
	pid_t pid = getpid();

	linkname<<"/proc/"<<pid<<"/exe";


	/* Now read the symbolic link */
	long ret = readlink(linkname.str().c_str(), buff, size);

	/* In case of an error, leave the handling up to the caller */
	if (ret == -1)
	{
		throw "Eeek";
	}
	/* Report insufficient buffer size */
	if (ret >= size)
	{
		throw "Eeek";
	}

	/* Ensure proper NUL termination */
	buff[ret] = 0;
	retVal=buff;
	return Glib::path_get_dirname(retVal);
}

int OSAbstraction::getPID()
{
	return getpid();
}

void OSAbstraction::showURL(std::string url)
{
	std::stringstream command;
	command<<"xdg-open "<<url<<endl;
	system(command.str().c_str());
}
