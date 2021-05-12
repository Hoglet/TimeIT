/*
 * Main.h
 *
 *  Created on: Mar 30, 2009
 *      Author: hoglet
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <libtimeit/db/database.h>
#include <libtimeit/timer.h>

namespace gui
{

class Main
{
public:
	Main(int argc, char *argv[]); // NOLINT(modernize-avoid-c-arrays)
	static void print_help();
	int run(int argc, char *argv[]); // NOLINT(modernize-avoid-c-arrays)
private:
	std::string db_name;
	std::string socket_name = "timeit.socket";
};

}
#endif /* MAIN_H_ */
