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

#include <window_manager.h>
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

Main::Main(int argc, char *argv[]) // NOLINT(modernize-avoid-c-arrays)
{
	signal(SIGINT, &sighandler);

	string db_path = Glib::build_filename(Glib::get_user_config_dir(), "TimeIT");
	make_directory(db_path);

	db_name = Glib::build_filename(db_path, "TimeIt.db");

	for (int i = 0; i < argc; i++)
	{
		string argument = argv[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
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

int Main::run(int argc, char *argv[]) // NOLINT(modernize-avoid-c-arrays)
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
			gui::Message_center message_center;

			GTK_timer timer;


			Network network;
			Sync_manager sync_manager(database, network, notifier, timer);
			Ipc_server ipc_server(socket_name, timer);

			Time_keeper  time_keeper(database, timer, notifier);
			Auto_tracker auto_tracker(time_keeper, database, timer);
			Window_manager   gui_factory(time_keeper, database, timer, notifier);
			Controller   controller(gui_factory, time_keeper, database, ipc_server, notifier);

			controller.start();

			//Then start message loop
			Gtk::Main::run();

		}
		else
		{
			Ipc_client ipc_client(socket_name);
			ipc_client.window_2_front();
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