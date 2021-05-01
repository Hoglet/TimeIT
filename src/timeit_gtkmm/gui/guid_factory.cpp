#include "guid_factory.h"
#include "main_window/main_window.h"
#include "timeit_about_dialog.h"
#include "edit_task_dialog.h"
#include "add_time.h"
#include "idle_dialog.h"
#include "status_icon.h"
#include "details_dialog.h"
#include "preference_dialog.h"
#include <memory>

namespace GUI
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

GUIFactory::~GUIFactory()
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
			shared_ptr<IdleDialog> dialog(new IdleDialog(timer, database, timeKeeper));
			retVal = dialog;
			dialogs.push_back(retVal);
			dialog->signal_hide().connect([this, dialog]() { this->on_dialog_hide(dialog); } );
		}
		break;
	case DETAILS_DIALOG:
		if (detailsDialogInstance == 0)
		{
			shared_ptr<DetailsDialog> dialog = DetailsDialog::create(database, timeKeeper, notifier);
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

void GUIFactory::on_dialog_hide(const WidgetPtr ptr)
{
	dialogs.remove(ptr);
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
		statusIcon = (StatusIcon*) (new StatusIcon(timeKeeper, database, notifier));
	}
	return *statusIcon;
}

WidgetPtr GUIFactory::getAddTime(int64_t taskID)
{
	if (addTimeInstance == nullptr)
	{
		if (mainWindow == 0)
		{
			getWidget(MAIN_WINDOW);
		}
		ICalendar &calendar = std::dynamic_pointer_cast<IMainWindow>(mainWindow)->getCalendar();
		shared_ptr<AddTime> addTime(new AddTime(taskID, calendar, database));
		addTime->signal_response().connect(sigc::mem_fun(this, &GUIFactory::on_addTime_response));
		addTimeInstance = addTime;
	}
	return addTimeInstance;
}

void GUIFactory::on_addTime_response(int)
{
	addTimeInstance->hide();
	addTimeInstance.reset();
}

}

