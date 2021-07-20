#ifndef DETAILS_DIALOG_H_
#define DETAILS_DIALOG_H_

#include <gtkmm.h>
#include "gui/widgets/lz_spin_button.h"
#include "details.h"
#include "main_window/summary.h"
#include <libtimeit/db/database.h>
#include <IWidget.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>

namespace gui
{
using namespace libtimeit;


class details_dialog:
		public Gtk::Dialog,
		public summary_observer,
		public event_observer,
		public time_keeper_observer,
		public IWidget
{
public:
	details_dialog(
			database&       db,
			Time_keeper&    time_keeper,
			notification_manager&       notifier,
			window_manager& window_manager,
			image_cache&         images);
	details_dialog(const details_dialog&) = delete;
	details_dialog(details_dialog&&) = delete;
	details_dialog& operator=(const details_dialog&) = delete;
	details_dialog& operator=(details_dialog&&) = delete;
	~details_dialog() override ;
	void set(int64_t ID,time_t startTime,time_t stopTime);


	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };

	void on_running_changed() override;
	void on_task_name_updated(int64_t task_id);
	void on_task_total_time_updated(int64_t task_id);

private:

	//SummaryObserver
	void on_selection_changed(int64_t ID,time_t startTime,time_t stopTime) override;
	void on_show_details_clicked(Task_id /*id*/, time_t /*start*/, time_t /*stop*/) override {};

	//EventObserver
	void on_task_time_changed(int64_t task_ID) override;
	void on_task_name_changed(int64_t task_ID) override;


	Gtk::Table table1;
	Gtk::Label task_name;
	Gtk::Image running_image;
	details    detail_list;
	Gtk::Label task_total_time;
	Glib::RefPtr<Gdk::Pixbuf> running_icon;
	Glib::RefPtr<Gdk::Pixbuf> running_idle_icon;
	Glib::RefPtr<Gdk::Pixbuf> blank_icon;
	Gtk::Table table2;
	Gtk::ScrolledWindow scrolled_window;


	time_t  range_start    {0};
	time_t  range_stop     {0};
	int64_t presented_task {0};
	int64_t time_entry_id  {0};
	time_accessor          times;
	extended_task_accessor tasks;
	settings_accessor      settings;
	Time_keeper&           time_keeper;
};
}

#endif /* DETAILS_DIALOG_H_ */
