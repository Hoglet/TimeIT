/*
 * Main.cpp
 *
 *  Created on: Mar 30, 2009
 *      Author: hoglet
 */

#include "main.h"

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif
#include <csignal>
#include <glibmm.h>
#include <glibmm/i18n.h>

#include <gui_factory.h>
#include <controller.h>
#include <MessageCenter.h>
#include <GTK_timer.h>

#include <libtimeit.h>
#include <libtimeit/logic/auto_tracker.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/misc/application_lock.h>
#include <libtimeit/misc/ipc_server.h>
#include <libtimeit/misc/ipc_client.h>
#include <libtimeit/OS_abstraction.h>
#include <libtimeit/sync/sync_manager.h>

namespace gui
{

using namespace std;
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

	string db_path = Glib::build_filename(Glib::get_user_config_dir(), "TimeIT");
	make_directory(db_path);

	db_name = Glib::build_filename(db_path, "TimeIt.db");

	for (int i = 0; i < argc; i++)
	{
		string argument = argv[i];
		if (argument == "--help" || argument == "-?")
		{
			print_help();
		}
		if (argument.substr(0, 5) == "--db=")
		{
			string filename = argument.substr(5, argument.length() - 5);
			if (filename.length() > 0)
			{
				db_name = filename;
				string tmp = db_name;
				replace(tmp.begin(), tmp.end(), ' ', '.');
				replace(tmp.begin(), tmp.end(), '/', '_');
				socket_name = tmp + ".socket";
			}
		}
	}

}

void Main::print_help()
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

		Application_lock lock(db_name);
		if (lock.lock_acquired())
		{

			Gtk::Main application(argc, argv);
			Gtk::Main::init_gtkmm_internals();
			libtimeit::init();
			Notifier notifier;

			//Create a database object
			Database database(db_name, notifier);

			//Initiate all logic
			Utils::MessageCenter message_center;

			GTK_timer timer;


			Network network;
			Sync_manager syncManager(database, network, notifier, timer);
			IpcServer ipcServer(socket_name, timer);

			Time_keeper  time_keeper(database, timer, notifier);
			Auto_tracker autotracker(time_keeper, database, timer);
			GUIFactory   guiFactory(time_keeper, database, timer, notifier);
			Controller   controller(guiFactory, time_keeper, database, ipcServer, notifier);

			controller.start();

			//Then start message loop
			Gtk::Main::run();

		}
		else
		{
			IpcClient ipcClient(socket_name);
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
	gui::Main program(argc, argv);
	return program.run(argc, argv);
}