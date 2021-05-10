#include "gui_factory.h"
#include "main_window/main_window.h"
#include "timeit_about_dialog.h"
#include "edit_task_dialog.h"
#include "edit_time.h"
#include "idle_dialog.h"
#include "status_icon.h"
#include "details_dialog.h"
#include "preference_dialog.h"
#include <memory>

namespace gui
{
using namespace libtimeit;
using namespace std;

//std::shared_ptr<Gtk::Main> GUIFactory::main;

GUIFactory::GUIFactory(
		Time_keeper &op_timeKeeper,
		Database &op_database,
		Timer& op_timer,
		Notifier& publisher)
		:
		timeKeeper(op_timeKeeper),
		database(op_database),
		timer(op_timer),
		notifier(publisher)

{

}

void GUIFactory::quit()
{
	Gtk::Main::quit();
}

WidgetPtr GUIFactory::getWidget(EWidget widget)
{
	WidgetPtr retVal;
	switch (widget)
	{
	case MAIN_WINDOW:
		if (mainWindow == 0)
		{
			shared_ptr<MainWindow> window(new MainWindow(database, timeKeeper, notifier));
			window->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_mainWindow_hide));
			this->mainWindow = window;

		}
		retVal = mainWindow;
		break;
	case ADD_TASK_DIALOG:
	case EDIT_TASK_DIALOG:
		if (editTaskDialogInstace == 0)
		{
			shared_ptr<Edit_task_dialog> dialog(new Edit_task_dialog(database));
			dialog->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_editTask_hide));
			editTaskDialogInstace = dialog;
		}
		retVal = editTaskDialogInstace;
		break;
	case ABOUT_DIALOG:
		if (aboutDialogInstance == 0)
		{
			shared_ptr<TimeItAboutDialog> dialog(new TimeItAboutDialog());
			dialog->signal_response().connect(sigc::mem_fun(this, &GUIFactory::on_aboutDialog_response));
			aboutDialogInstance = dialog;
		}
		retVal = aboutDialogInstance;
		break;
	case IDLE_DIALOG:
		{
			auto dialog = make_shared<IdleDialog>(timer, database, timeKeeper);
			manage_lifespan(dialog);
			retVal = dialog;
		}
		break;
	case DETAILS_DIALOG:
		if (detailsDialogInstance == 0)
		{
			shared_ptr<DetailsDialog> dialog = DetailsDialog::create(database, timeKeeper, notifier, *this);
			dialog->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_detailsDialog_hide));
			detailsDialogInstance = dialog;
		}
		retVal = detailsDialogInstance;
		break;
	case PREFERENCE_DIALOG:
		if (preferenceDialogInstance == 0)
		{
			shared_ptr<PreferenceDialog> dialog(new PreferenceDialog(database));
			dialog->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_preferenceDialog_hide));
			preferenceDialogInstance = dialog;
		}
		retVal = preferenceDialogInstance;
		break;
	case MAX_WIDGETS:
	default:
		break;
	}
	return retVal;
}
void GUIFactory::on_mainWindow_hide()
{
	//mainWindow.reset();
}
void GUIFactory::on_addTaskDialog_hide()
{
	addTaskInstance.reset();
}

void GUIFactory::on_dialog_hide(shared_ptr<Gtk::Dialog> dialog)
{
	dialogs.remove(dialog);
}

void GUIFactory::on_detailsDialog_hide()
{
	detailsDialogInstance.reset();
}

void GUIFactory::on_aboutDialog_response(int)
{
	aboutDialogInstance->hide();
	aboutDialogInstance.reset();
}

void GUIFactory::on_editTask_hide()
{
	editTaskDialogInstace.reset();
}

void GUIFactory::on_preferenceDialog_hide()
{
	preferenceDialogInstance.reset();
}

StatusIcon& GUIFactory::getStatusIcon()
{
	static StatusIcon *statusIcon = 0;
	if (statusIcon == 0)
	{
		statusIcon =  (new StatusIcon(timeKeeper, database, notifier));
	}
	return *statusIcon;
}




void GUIFactory::manage_lifespan(shared_ptr<Gtk::Dialog> dialog)
{
	dialog->signal_hide().connect([this, dialog]() { this->on_dialog_hide(dialog); } );
	dialog->signal_response().connect([this, dialog](int) { this->on_dialog_response(dialog); });
	dialogs.push_back(dialog);
}


void GUIFactory::on_addTime_response(int)
{
	addTimeInstance->hide();
	addTimeInstance.reset();
}

void GUIFactory::on_dialog_response(shared_ptr<Gtk::Dialog> dialog)
{
	dialog->hide();
}

}

