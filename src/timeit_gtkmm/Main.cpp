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
#include <glibmm.h>
#include <glibmm/i18n.h>

#include <GUIFactory.h>
#include <Controller.h>
#include <MessageCenter.h>
#include <GTK_timer.h>

#include <libtimeit.h>
#include <libtimeit/logic/AutoTracker.h>
#include <libtimeit/logic/TimeKeeper.h>
#include <libtimeit/logic/TimeKeeper.h>
#include <libtimeit/misc/ApplicationLock.h>
#include <libtimeit/misc/IpcServer.h>
#include <libtimeit/misc/IpcClient.h>
#include <libtimeit/OSAbstraction.h>
#include <libtimeit/sync/SyncManager.h>

namespace GUI
{

using namespace std;
using namespace Test;
using namespace libtimeit;

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

	string dbPath = Glib::build_filename(Glib::get_user_config_dir(), "TimeIT");
	OSAbstraction::mkDir(dbPath);

	dbName = Glib::build_filename(dbPath, "TimeIt.db");

	test = false;
	for (int i = 0; i < argc; i++)
	{
		string argument = argv[i];
		if (argument == "--help" || argument == "-?")
		{
			printHelp();
		}
		if (argument.substr(0, 5) == "--db=")
		{
			string filename = argument.substr(5, argument.length() - 5);
			if (filename.length() > 0)
			{
				dbName = filename;
				string tmp = dbName;
				replace(tmp.begin(), tmp.end(), ' ', '.');
				replace(tmp.begin(), tmp.end(), '/', '_');
				socketName = tmp + ".socket";
			}
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
			Notifier notifier;

			//Create a database object
			Database database(dbName, notifier);

			//Initiate all logic
			Utils::MessageCenter messageCenter();

			GTK_timer timer;


			Timekeeper time_keeper(database, timer, notifier);
			GUIFactory guiFactory(time_keeper, database, timer, notifier);
			AutoTracker autotracker(time_keeper, database, timer);
			Network network;
			SyncManager syncManager(database, network, notifier, timer);
			IpcServer ipcServer(socketName, timer);
			Controller controller(guiFactory, time_keeper, database, ipcServer, notifier);

			controller.start();

			//Then start message loop
			application.run();

		}
		else
		{
			IpcClient ipcClient(socketName);
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

} // namespace GUI

int main(int argc, char *argv[])
{
	GUI::Main program(argc, argv);
	return program.run(argc, argv);
}