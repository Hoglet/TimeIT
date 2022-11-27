#include "window_manager.h"
#include "main_window/main_window.h"
#include "about_dialog.h"
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

window_manager::window_manager( time_manager& op_time_keeper, database& op_database, Timer& op_timer,
								notification_manager& op_notifier, image_cache& op_images,
								controller_interface& op_controller)
		:
		time_keeper( op_time_keeper ),
		db( op_database ),
		timer( op_timer ),
		notifier( op_notifier ),
		images( op_images ),
		controller_( op_controller )
{}

void window_manager::quit()
{
	if(main_window_instance != nullptr)
	{
		main_window_instance->hide();
	}
	Gtk::Main::quit();
}

widget_ptr window_manager::get_widget( widget_type widget)
{
	widget_ptr ret_val;
	switch (widget)
	{
	case MAIN_WINDOW:
		if (main_window_instance == nullptr)
		{
			shared_ptr<main_window> window(new main_window(db, time_keeper, notifier, *this, images, controller_));
			window->signal_hide().connect(sigc::mem_fun(this, &window_manager::on_main_window_hide));
			this->main_window_instance = window;

		}
		ret_val = main_window_instance;
		break;
	case ADD_TASK_DIALOG:
	case EDIT_TASK_DIALOG:
		if (edit_task_dialog_instace == nullptr)
		{
			shared_ptr<edit_task_dialog> dialog(new edit_task_dialog(db));
			dialog->signal_hide().connect(sigc::mem_fun(this, &window_manager::on_edit_task_hide));
			edit_task_dialog_instace = dialog;
		}
		ret_val = edit_task_dialog_instace;
		break;
	case ABOUT_DIALOG:
		if (about_dialog_instance == nullptr)
		{
			shared_ptr<about_dialog> dialog(new about_dialog(images));
			dialog->signal_response().connect(sigc::mem_fun(this, &window_manager::on_about_dialog_response));
			about_dialog_instance = dialog;
		}
		ret_val = about_dialog_instance;
		break;
	case IDLE_DIALOG:
		{
			auto dialog = make_shared<idle_dialog>(timer, db, time_keeper);
			manage_lifespan(dialog);
			ret_val = dialog;
		}
		break;
	case PREFERENCE_DIALOG:
		if (preference_dialog_instance == nullptr)
		{
			shared_ptr<preference_dialog> dialog(new preference_dialog(db));
			dialog->signal_hide().connect(sigc::mem_fun(this, &window_manager::on_preference_dialog_hide));
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
void window_manager::on_main_window_hide()
{
	//mainWindow.reset();
}
void window_manager::on_add_task_dialog_hide()
{
	add_task_instance.reset();
}

void window_manager::on_dialog_hide(Gtk::Dialog* dialog)
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


void window_manager::on_about_dialog_response(int /*response*/)
{
	about_dialog_instance->hide();
	about_dialog_instance.reset();
}

void window_manager::on_edit_task_hide()
{
	edit_task_dialog_instace.reset();
}

void window_manager::on_preference_dialog_hide()
{
	preference_dialog_instance.reset();
}

status_icon_widget& window_manager::get_status_icon()
{
	if (status_icon_instance == nullptr)
	{
		status_icon_instance =  (new status_icon_widget(time_keeper, db, notifier, images, controller_));
	}
	return *status_icon_instance;
}




void window_manager::manage_lifespan(shared_ptr<Gtk::Dialog> dialog)
{
	auto* raw_pointer = dialog.get();
	dialog->signal_hide().connect([this, raw_pointer]() { this->on_dialog_hide(raw_pointer); } );
	dialog->signal_response().connect([raw_pointer](int /*response*/) { window_manager::on_dialog_response(raw_pointer); });
	active_dialogs.push_back(dialog);
}


void window_manager::on_dialog_response(Gtk::Dialog* dialog)
{
	dialog->hide();
}

}
