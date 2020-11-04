/*
 * Main.h
 *
 *  Created on: Mar 30, 2009
 *      Author: hoglet
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <Database.h>
#include <Timer.h>
#include <IGUIFactory.h>

class Main
{
public:
	Main(int argc, char *argv[]);
	virtual ~Main();
	void printHelp();
	int run(int argc, char *argv[]);
private:
	std::shared_ptr<DB::IDatabase> database;
	std::shared_ptr<Timer> timer;
	std::shared_ptr<GUI::IGUIFactory> guiFactory;
	std::string dbName;
	std::string socketName = "timeit.socket";
	bool test;
};

#endif /* MAIN_H_ */
