#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_

#include <gtkmm.h>
#include <IWidget.h>
#include <main_window/main_window.h>
#include "task_list.h"
#include <libtimeit/db/extended_task.h>
#include <libtimeit/db/settings_accessor.h>
#include "summary_views.h"
#include "timeit_about_dialog.h"
#include "details.h"
#include "menu.h"
#include "toolbar.h"
#include "calendar.h"

namespace gui
{
using namespace libtimeit;
using namespace std;

class MainWindow: public Gtk::Window, public action_observer, public IWidget
{
public:
	~MainWindow() override;
	MainWindow(Database& database, Time_keeper &timeKeeper, Notifier& notifier);


	// IWidget interface
	void show() override
	{
		Gtk::Window::deiconify();
		Gtk::Window::show();
	}
	void hide() override
	{
		Gtk::Window::hide();
	}
	bool is_visible() override
	{
		return Gtk::Window::is_visible();
	}
	;
	void move(int x, int y) override
	{
		Gtk::Window::move(x, y);
	}
	;
	void get_position(int &Window_x, int &Window_y) override
	{
		Gtk::Window::get_position(Window_x, Window_y);
	}
	;

	virtual void attach(SummaryObserver* observer);
	virtual void detach(SummaryObserver* observer);
	void attach(action_observer* /*observer*/) override;
	void detach(action_observer* /*observer*/) override;

	Calendar& get_calendar();

	void on_show() override;
	void on_running_tasks_changed();

private:
	//Action observer
	void on_action_task_selection_changed(int selectedTaskID) override;
	void on_action_remove_task() override;

	virtual void on_settings_changed(string name);


//	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime) {};
//	virtual void on_showDetailsClicked(ISummary* summary, int64_t taskId, time_t startTime, time_t stopTime);
	//
	void create_layout();
	void relate_widgets();
	void set_calendar();

	void default_layout();
	void classic_layout();
	void save_size();

	static void remove_children(Container& container);
	void empty_containers();
	void do_layout();

	TaskList       task_list;
	DaySummary     day_summary;
	WeekSummary    week_summary;
	MonthSummary   month_summary;
	YearSummary    year_summary;
	Calendar       calendar;
	Gtk::Notebook  summary_tabs;

	Menu    menubar;
	Toolbar toolbar;

	Gtk::ScrolledWindow  task_list_container;
	Gtk::ScrolledWindow  day_summary_container;
	Gtk::Label           label_day;
	Gtk::ScrolledWindow  week_summary_container;
	Gtk::Label           label_week;
	Gtk::ScrolledWindow  month_summary_container;
	Gtk::Label           label_month;
	Gtk::ScrolledWindow  year_summary_container;
	Gtk::Label           label_year;

	Gtk::VBox      secondary_v_box; //on right side
	Gtk::HPaned    h_paned;
	Gtk::Statusbar statusbar;
	Gtk::VBox      main_v_box;

	Time_accessor          time_accessor;
	Extended_task_accessor task_accessor;
	Settings_accessor      settings_accessor;
	vector<Summary*>       summaries;
};
}
#endif // _MAIN_WINDOW_HPP_
