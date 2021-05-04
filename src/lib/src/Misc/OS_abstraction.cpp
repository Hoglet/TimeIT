/*
 * OS_abstraction.cpp
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#include "libtimeit/OS_abstraction.h"
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

namespace libtimeit
{

//LCOV_EXCL_START
void make_directory(string path)
{
	stringstream mkdir_command;
	mkdir_command << "mkdir -p \"" << path << "\"";
	system(mkdir_command.str().c_str());
}
//LCOV_EXCL_STOP

void unlink(string filename)
{
	::unlink(filename.c_str());
}

// Code converted from http://www.techbytes.ca/techbyte103.html
bool file_exists(string filename)
{
	struct stat st_file_info{};

	// Attempt to get the file attributes
	int int_stat = stat(filename.c_str(), &st_file_info);

	return (int_stat == 0);
}

//LCOV_EXCL_START

void show_URL(string url)
{
	stringstream command;
	command << "xdg-open " << url << endl;
	system(command.str().c_str());
}
//LCOV_EXCL_STOP
}
