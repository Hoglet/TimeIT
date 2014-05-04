#include "ControllerTest.h"
#include "cute.h"
#include "Controller.h"
#include "Mock/MockTimeKeeper.h"
#include "Mock/MockGuiFactory.h"
#include "Mock/MockDatabase.h"
#include "DB/TempDB.h"
#include "Timer.h"
#include "ITimeKeeper.h"
#include "Controller.h"

using namespace std;

namespace Test
{

void Controller_startSequence()
{
	//Create a database object
	shared_ptr<DB::IDatabase> database = std::shared_ptr<DB::IDatabase>(new TempDB());
	shared_ptr<Timer> timer = std::shared_ptr<Timer>(new Timer());
	shared_ptr<ITimeKeeper> timekeeper = std::shared_ptr<ITimeKeeper>(new MockTimeKeeper());
	shared_ptr<MockGuiFactory> guiFactory= std::shared_ptr<MockGuiFactory>(new MockGuiFactory());
	shared_ptr<Utils::MessageCenter> messageCenter= std::shared_ptr<Utils::MessageCenter>(new Utils::MessageCenter());
	MockStatusIcon& statusIcon = static_cast<MockStatusIcon&>(guiFactory->getStatusIcon());

	ASSERT_EQUALM("Checking status of status icon before start sequence ", false, statusIcon.visible);
	ASSERT_EQUALM("Checking status of widgetIdentifier before start sequence ", GUI::MAX_WIDGETS, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget before start sequence ", false, guiFactory->widget->is_visible());

	shared_ptr<GUI::IGUIFactory> gfactory = std::static_pointer_cast<GUI::IGUIFactory>(guiFactory);
	Controller controller(gfactory, timekeeper, database, messageCenter);
	controller.start();
	ASSERT_EQUALM("Checking status of status icon after start sequence ", true, statusIcon.visible);
	ASSERT_EQUALM("Checking status of widgetIdentifier after start sequence ", GUI::MAIN_WINDOW, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget after start sequence ", true, guiFactory->widget->is_visible());

}

void Controller_testActions()
{
	//Create a database object
	shared_ptr<DB::IDatabase> database = std::shared_ptr<DB::IDatabase>(new TempDB());
	shared_ptr<Timer> timer = std::shared_ptr<Timer>(new Timer());
	shared_ptr<MockTimeKeeper> timekeeper = std::shared_ptr<MockTimeKeeper>(new MockTimeKeeper());
	shared_ptr<MockGuiFactory> guiFactory= std::shared_ptr<MockGuiFactory>(new MockGuiFactory());
	shared_ptr<Utils::MessageCenter> messageCenter= std::shared_ptr<Utils::MessageCenter>(new Utils::MessageCenter());

	shared_ptr<GUI::IGUIFactory> gfactory = std::static_pointer_cast<GUI::IGUIFactory>(guiFactory);
	shared_ptr<ITimeKeeper> tkeeper = std::shared_ptr<ITimeKeeper>(timekeeper);

	Controller controller(gfactory, tkeeper, database, messageCenter);

	ASSERT_EQUALM("Checking status of widgetIdentifier before about event ", GUI::MAX_WIDGETS, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget before about event ", false, guiFactory->widget->is_visible());
	controller.on_action_about();
	ASSERT_EQUALM("Checking status of widgetIdentifier after about event ", GUI::ABOUT_DIALOG, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget after about event", true, guiFactory->widget->is_visible());

	guiFactory->widget->hide();
	ASSERT_EQUALM("Checking status of mockWidget before add task event ", false, guiFactory->widget->is_visible());
	controller.on_action_add_task();
	ASSERT_EQUALM("Checking status of widgetIdentifier after add task event ", GUI::ADD_TASK_DIALOG, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget after add task event", true, guiFactory->widget->is_visible());

	guiFactory->widget->hide();
	ASSERT_EQUALM("Checking status of mockWidget before preferences event ", false, guiFactory->widget->is_visible());
	controller.on_action_preferences();
	ASSERT_EQUALM("Checking status of widgetIdentifier after preferences event ", GUI::PREFERENCE_DIALOG, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget after preferences event", true, guiFactory->widget->is_visible());

	guiFactory->widget->hide();
	ASSERT_EQUALM("Checking status of mockWidget before edit task event ", false, guiFactory->widget->is_visible());
	controller.on_action_edit_task();
	ASSERT_EQUALM("Checking status of widgetIdentifier after edit task event ", GUI::EDIT_TASK_DIALOG, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget after edit task event", true, guiFactory->widget->is_visible());

	guiFactory->widget->hide();
	ASSERT_EQUALM("Checking status of mockWidget before toggling main window ", false, guiFactory->widget->is_visible());
	controller.on_action_toggleMainWindow();
	ASSERT_EQUALM("Checking status of widgetIdentifier after toggling main window ", GUI::MAIN_WINDOW, guiFactory->widgetIdentifier);
	ASSERT_EQUALM("Checking status of mockWidget after toggling main window", true, guiFactory->widget->is_visible());
	controller.on_action_toggleMainWindow();
	ASSERT_EQUALM("Checking status of mockWidget after toggling main window again", false, guiFactory->widget->is_visible());

	int selectedTaskId=4;
	timekeeper->startedTask=0;
	controller.on_action_task_selection_changed(selectedTaskId);
	controller.on_action_start_task();
	ASSERT_EQUALM("Check that controller is starting the correct task ", selectedTaskId, timekeeper->startedTask );

	selectedTaskId++;
	controller.on_action_task_selection_changed(selectedTaskId);
	controller.on_action_stop_task();
	ASSERT_EQUALM("Check that controller is starting the correct task ", selectedTaskId, timekeeper->stopedTask );

}


cute::suite make_suite_ControllerTest()
{
	cute::suite s;
	s.push_back(CUTE(Controller_startSequence));
	s.push_back(CUTE(Controller_testActions));
	return s;
}

} /* namespace Test */
