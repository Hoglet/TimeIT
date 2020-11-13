#include "GUIFactory.h"
#include "MainWindow/MainWindow.h"
#include "TimeItAboutDialog.h"
#include "EditTaskDialog.h"
#include "AddTime.h"
#include "IdleDialog.h"
#include "StatusIcon.h"
#include "DetailsDialog.h"
#include "PreferenceDialog.h"
#include <memory>

namespace GUI
{

//std::shared_ptr<Gtk::Main> GUIFactory::main;

GUIFactory::GUIFactory(std::shared_ptr<ITimeKeeper> &op_timekeeper, std::shared_ptr<DB::IDatabase> &op_database, Timer& op_timer) :
		timekeeper(op_timekeeper), database(op_database), timer(op_timer)
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
			std::shared_ptr<MainWindow> window(new MainWindow(database));
			window->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_mainWindow_hide));
			this->mainWindow = window;

		}
		retVal = mainWindow;
		break;
	case ADD_TASK_DIALOG:
	case EDIT_TASK_DIALOG:
		if (editTaskDialogInstace == 0)
		{
			std::shared_ptr<EditTaskDialog> dialog(new EditTaskDialog(database));
			dialog->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_editTask_hide));
			editTaskDialogInstace = dialog;
		}
		retVal = editTaskDialogInstace;
		break;
	case ABOUT_DIALOG:
		if (aboutDialogInstance == 0)
		{
			std::shared_ptr<TimeItAboutDialog> dialog(new TimeItAboutDialog());
			dialog->signal_response().connect(sigc::mem_fun(this, &GUIFactory::on_aboutDialog_response));
			aboutDialogInstance = dialog;
		}
		retVal = aboutDialogInstance;
		break;
	case IDLE_DIALOG:
		if (idleDialogInstance == 0)
		{
			std::shared_ptr<IdleDialog> dialog(new IdleDialog(timer, database->getTaskAccessor()));
			dialog->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_idleDialog_hide));
			idleDialogInstance = dialog;
		}
		retVal = idleDialogInstance;
		break;
	case DETAILS_DIALOG:
		if (detailsDialogInstance == 0)
		{
			std::shared_ptr<DetailsDialog> dialog = DetailsDialog::create(database);
			dialog->signal_hide().connect(sigc::mem_fun(this, &GUIFactory::on_detailsDialog_hide));
			detailsDialogInstance = dialog;
		}
		retVal = detailsDialogInstance;
		break;
	case PREFERENCE_DIALOG:
		if (preferenceDialogInstance == 0)
		{
			std::shared_ptr<PreferenceDialog> dialog(new PreferenceDialog(database));
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

void GUIFactory::on_idleDialog_hide()
{
	idleDialogInstance.reset();
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

IStatusIcon& GUIFactory::getStatusIcon()
{
	static IStatusIcon *statusIcon = 0;
	if (statusIcon == 0)
	{
		std::shared_ptr<DB::ITaskAccessor> taskaccessor = database->getTaskAccessor();
		std::shared_ptr<DB::ITimeAccessor> timeaccessor = database->getTimeAccessor();

		statusIcon = (IStatusIcon*) (new StatusIcon(timekeeper, taskaccessor, timeaccessor));
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
		std::shared_ptr<AddTime> addTime(new AddTime(taskID, calendar, database));
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

