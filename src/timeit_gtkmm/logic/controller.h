#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <action_observer.h>
#include <window_manager.h>
#include <idle_dialog.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>
#include <main_window/summary.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/database.h>
#include <libtimeit/event_observer.h>

namespace gui
{

using namespace std;
using namespace libtimeit;

class widget_controller : // NOLINT(cppcoreguidelines-special-member-functions)
		public action_observer,
		public time_keeper_observer,
		public summary_observer,
		public event_observer
{
public:
	widget_controller(
			window_manager& op_gui_factory,
			Time_keeper&    op_time_keeper,
			database&       db,
			notification_manager&       op_notifier,
			image_cache&         images);
	~widget_controller() override;
	void start();

	//Action observers
	void on_action_task_selection_changed(optional<task_id> selected_task_id) override;
	void on_action_add_task() override;
	void on_action_remove_task() override;
	void on_action_add_time() override;
	void on_action_start_task() override;
	void on_action_stop_task() override;
	void on_action_edit_task() override;
	void on_action_about() override;
	void on_action_help() override;
	void on_action_quit() override;
	void on_action_preferences() override;
	void on_action_report_bug() override;

	//SummaryObserver
	void on_show_details_clicked(
			const task_id&           id,
			time_point<system_clock> start_time,
			time_point<system_clock> stop_time) override;

	void on_selection_changed(
			optional<task_id>        /*id*/,
			time_point<system_clock> /*start*/,
			time_point<system_clock> /*stop*/) override
			{};

	//
	void on_idle_detected(const time_entry& /*te*/ ) override;
	void on_idle_changed();

	void on_time_entry_changed(const time_entry& /*te*/) override;
	void on_running_changed() override;
	void on_action_toggle_main_window() override;
	void on_show_main_window() override;
	void on_action_stop_timers() override;

private:
	window_manager&          windows;
	Time_keeper&             time_keeper;
	time_accessor            times;
	settings_accessor        settings;
	database&                db;
	notification_manager&    notifier;
	image_cache&             images;

	int main_window_x = 0;
	int main_window_y = 0;
	optional<task_id> selected_task_id {};
	vector<task_id> old_running{};

	void show_idle_dialog(const time_id& id);
};
}
#endif /* CONTROLLER_H */
