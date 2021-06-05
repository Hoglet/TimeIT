#include "window_manager.h"
#include "main_window/main_window.h"
#include "timeit_about_dialog.h"
#include "edit_task_dialog.h"
#include "idle_dialog.h"
#include "status_icon.h"
#include "preference_dialog.h"
#include <memory>

namespace gui
{
using namespace libtimeit;
using namespace std;

//std::shared_ptr<Gtk::Main> GUIFactory::main;

Window_manager::Window_manager(
		Time_keeper& op_timeKeeper,
		Database&    op_database,
		Timer&       op_timer,
		Notifier&    publisher,
		Images&      images_)
		:
		time_keeper(op_timeKeeper),
		database(op_database),
		timer(op_timer),
		notifier(publisher),
		images(images_)

{

}

void Window_manager::quit()
{
	if(main_window!=nullptr)
	{
		main_window->hide();
	}
	Gtk::Main::quit();
}

WidgetPtr Window_manager::get_widget(EWidget widget)
{
	WidgetPtr ret_val;
	switch (widget)
	{
	case MAIN_WINDOW:
		if (main_window == nullptr)
		{
			shared_ptr<MainWindow> window(new MainWindow(database, time_keeper, notifier, *this, images));
			window->signal_hide().connect(sigc::mem_fun(this, &Window_manager::on_main_window_hide));
			this->main_window = window;

		}
		ret_val = main_window;
		break;
	case ADD_TASK_DIALOG:
	case EDIT_TASK_DIALOG:
		if (edit_task_dialog_instace == nullptr)
		{
			shared_ptr<Edit_task_dialog> dialog(new Edit_task_dialog(database));
			dialog->signal_hide().connect(sigc::mem_fun(this, &Window_manager::on_edit_task_hide));
			edit_task_dialog_instace = dialog;
		}
		ret_val = edit_task_dialog_instace;
		break;
	case ABOUT_DIALOG:
		if (about_dialog_instance == nullptr)
		{
			shared_ptr<TimeItAboutDialog> dialog(new TimeItAboutDialog(images));
			dialog->signal_response().connect(sigc::mem_fun(this, &Window_manager::on_about_dialog_response));
			about_dialog_instance = dialog;
		}
		ret_val = about_dialog_instance;
		break;
	case IDLE_DIALOG:
		{
			auto dialog = make_shared<Idle_dialog>(timer, database, time_keeper);
			manage_lifespan(dialog);
			ret_val = dialog;
		}
		break;
	case PREFERENCE_DIALOG:
		if (preference_dialog_instance == nullptr)
		{
			shared_ptr<Preference_dialog> dialog(new Preference_dialog(database));
			dialog->signal_hide().connect(sigc::mem_fun(this, &Window_manager::on_preference_dialog_hide));
			preference_dialog_instance = dialog;
		}
		ret_val = preference_dialog_instance;
		break;
	case MAX_WIDGETS:
	default:
		break;
	}
	return ret_val;
}
void Window_manager::on_main_window_hide()
{
	//mainWindow.reset();
}
void Window_manager::on_add_task_dialog_hide()
{
	add_task_instance.reset();
}

void Window_manager::on_dialog_hide(Gtk::Dialog* dialog)
{
	for( auto item: active_dialogs)
	{
		if( item.get() == dialog)
		{
			active_dialogs.remove(item);
			break;
		}
	}
}


void Window_manager::on_about_dialog_response(int /*response*/)
{
	about_dialog_instance->hide();
	about_dialog_instance.reset();
}

void Window_manager::on_edit_task_hide()
{
	edit_task_dialog_instace.reset();
}

void Window_manager::on_preference_dialog_hide()
{
	preference_dialog_instance.reset();
}

Status_icon& Window_manager::get_status_icon()
{
	static Status_icon *status_icon = nullptr;
	if (status_icon == nullptr)
	{
		status_icon =  (new Status_icon(time_keeper, database, notifier, images));
	}
	return *status_icon;
}




void Window_manager::manage_lifespan(shared_ptr<Gtk::Dialog> dialog)
{
	auto raw_pointer = dialog.get();
	dialog->signal_hide().connect([this, raw_pointer]() { this->on_dialog_hide(raw_pointer); } );
	dialog->signal_response().connect([raw_pointer](int /*response*/) { Window_manager::on_dialog_response(raw_pointer); });
	active_dialogs.push_back(dialog);
}


void Window_manager::on_dialog_response(Gtk::Dialog* dialog)
{
	dialog->hide();
}

}

