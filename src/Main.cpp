/*
 * Main.cpp
 *
 *  Created on: Mar 30, 2009
 *      Author: hoglet
 */

#include "Main.h"

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif
#include <signal.h>
#include <GUIFactory.h>
#include <ApplicationLock.h>
#include <OSAbstraction.h>
#include <Controller.h>
#include <AutoTracker.h>

#include <glibmm.h>

#include <TimeKeeper.h>
#include <glibmm/i18n.h>
#include <TestRunner.h>
#include <SyncManager.h>
#include <MessageCenter.h>

using namespace std;
using namespace Test;

extern "C"
{
void sighandler(int sig)
{
	printf("signal %d caught...\n", sig);
	GUI::GUIFactory::quit();
}
}

Main::Main(int argc, char *argv[])
{
	GUI::GUIFactory::init(argc, argv);
	signal(SIGINT, &sighandler);

	std::string dbPath = Glib::build_filename(Glib::get_user_config_dir(), "TimeIT");
	OSAbstraction::mkDir(dbPath);

	dbName = Glib::build_filename(dbPath, "TimeIt.db");

	test = false;
	for (int i = 0; i < argc; i++)
	{
		std::string argument = argv[i];
		if (argument == "--help" || argument == "-?")
		{
			printHelp();
		}
		if (argument.substr(0, 5) == "--db=")
		{
			std::string filename = argument.substr(5, argument.length() - 5);
			if (filename.length() > 0)
			{
				dbName = filename;
			}
		}
		if (argument == "--test" || argument == "-t")
		{
			TestRunner testrunner;
			testrunner.run(argc, argv);
			exit(0);
		}
	}

}

Main::~Main()
{
}

void Main::printHelp()
{
	cout << _("Usage:")<<endl;
	cout << _("timeit [OPTION...]")<<endl;
	cout << endl;
	cout << _("Help Options:")<<endl;
	cout << _(" -?, --help                                Show help")<<endl;
	cout << _(" -t, --test                                Run tests and exit")<<endl;
	cout << _("--db=[FILENAME]")<<endl;
}

int Main::run()
{
	try
	{
		bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
		textdomain(GETTEXT_PACKAGE);

		ApplicationLock lock(dbName);
		if (lock.lockAquired())
		{
			//Create a database object
			database = std::shared_ptr<DB::IDatabase>(new DB::Database(dbName));

			//Initiate all logic
			std::shared_ptr<Utils::MessageCenter> messageCenter = std::shared_ptr<Utils::MessageCenter>(new Utils::MessageCenter());

			timer = std::shared_ptr<Timer>(new Timer());

			std::shared_ptr<ITimeKeeper> timekeeper = std::shared_ptr<ITimeKeeper>(new Timekeeper(database, timer));
			guiFactory = std::shared_ptr<GUI::IGUIFactory>(new GUI::GUIFactory(timekeeper, database, timer));

			AutoTracker autotracker(timekeeper, database, timer);

			std::shared_ptr<INetwork> network = std::shared_ptr<INetwork>(new Network(messageCenter));
			syncing::SyncManager syncManager(database, network);
			syncManager.start();

			Controller controller(guiFactory, timekeeper, database);
			controller.start();
			//Then start message loop
			guiFactory->run();
		}
		else
		{
			cerr << _("Application already running!\n");
		}
	} catch (exception& e)
	{
		cerr << _(e.what()) << endl;
		return 1;
	}
	return 0;

}

int main(int argc, char *argv[])
{
	Main program(argc, argv);
	return program.run();
}
