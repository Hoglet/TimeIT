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
	Main(int argc, char *argv[]);
	virtual ~Main();
	void printHelp();
	int run(int argc, char *argv[]);
private:
	std::string dbName;
	std::string socketName = "timeit.socket";
};

}
#endif /* MAIN_H_ */
