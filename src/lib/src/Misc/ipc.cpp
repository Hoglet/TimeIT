/*
 * Ipc.cpp
 *
 *  Created on: 17 Jul 2019
 *      Author: hoglet
 */

#include "libtimeit/misc/ipc.h"
#include <iostream>
#include <sys/stat.h>

namespace libtimeit
{

using namespace std;


std::string ipc::prepare_socket_dir()
{
	string runtime_dir;
	char *s = getenv("XDG_RUNTIME_DIR");   // NOLINT
	if (s != nullptr)
	{
		runtime_dir = s;
	}
	else
	{
		std::cerr << "XDG_RUNTIME_DIR is not defined, fallback to /tmp\n";
		runtime_dir = "/tmp";
	}
	runtime_dir = runtime_dir + "/timeit/";

	mkdir(runtime_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	return runtime_dir;
}

} /* namespace Utils */
