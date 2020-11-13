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
#include <libtimeit/OSAbstraction.h>
#include <Controller.h>
#include <AutoTracker.h>

#include <glibmm.h>

#include <TimeKeeper.h>
#include <glibmm/i18n.h>
#include <TestRunner.h>
#include <SyncManager.h>
#include <MessageCenter.h>
#include <IpcServer.h>
#include <IpcClient.h>
#include <libtimeit.h>

using namespace std;
using namespace Test;

extern "C" {
void sighandler(int sig)
{
	printf("signal %d caught...\n", sig);
	//GUI::GUIFactory::quit();
	Gtk::Main::quit();
}
}

Main::Main(int argc, char *argv[])
{
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
				std::string tmp = dbName;
				std::replace(tmp.begin(), tmp.end(), ' ', '.');
				std::replace(tmp.begin(), tmp.end(), '/', '_');
				socketName = tmp + ".socket";
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
	// Command line help, line1.
	cout << _("Usage:") << endl;
	// Command line help, Line2. Showing how to call the program. Will expand into "timeit [OPTION...]"
	cout << "timeit [" << _("OPTION...") << "]" << endl;
	cout << endl;
	// Command line help, Line3.
	cout << _("Help Options:") << endl;
	// Command line help, Line4. How to show help
	cout << " -?, --help                                " << _("Show help") << endl;
	// Command line help, how to run internal test
	cout << " -t, --test                                " << _("Run tests and exit") << endl;
	// Command line help, Line6. Will expand to "--db=[FILENAME]"
	cout << "--db=[" << _("FILENAME") << "]" << endl;
}

int Main::run(int argc, char *argv[])
{
	try
	{
		bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
		textdomain(GETTEXT_PACKAGE);

		ApplicationLock lock(dbName);
		if (lock.lockAquired())
		{

			Gtk::Main application(argc, argv);
			Gtk::Main::init_gtkmm_internals();
			libtimeit::init();

			//Create a database object
			database = std::shared_ptr<DB::IDatabase>(new DB::Database(dbName));

			//Initiate all logic
			std::shared_ptr<Utils::MessageCenter> messageCenter = std::shared_ptr<Utils::MessageCenter>(new Utils::MessageCenter());

			Timer timer;
			sigc::connection connection = Glib::signal_timeout().connect_seconds(sigc::mem_fun(&timer, &Timer::on_signal_1_second), 1);

			std::shared_ptr<ITimeKeeper> timekeeper = std::shared_ptr<ITimeKeeper>(new Timekeeper(database, timer));
			guiFactory = std::shared_ptr<GUI::IGUIFactory>(new GUI::GUIFactory(timekeeper, database, timer));

			AutoTracker autotracker(timekeeper, database, timer);

			std::shared_ptr<INetwork> network = std::shared_ptr<INetwork>(new Network());
			syncing::SyncManager syncManager(database, network, messageCenter);
			syncManager.start();

			std::shared_ptr<Utils::IpcServer> ipcServer = std::shared_ptr<Utils::IpcServer>(new Utils::IpcServer(socketName, timer));

			Controller controller(guiFactory, timekeeper, database, ipcServer);
			controller.start();

			//Then start message loop
			application.run();
			guiFactory.reset();
			connection.disconnect();
		}
		else
		{
			Utils::IpcClient ipcClient(socketName);
			ipcClient.window2front();
		}
	}
	catch (exception &e)
	{
		cerr << _(e.what()) << endl;
		return 1;
	}
	return 0;

}

int main(int argc, char *argv[])
{
	Main program(argc, argv);
	return program.run(argc, argv);
}
