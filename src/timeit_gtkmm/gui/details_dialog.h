#ifndef DETAILS_DIALOG_H
#define DETAILS_DIALOG_H

#include <gtkmm.h>
#include "gui/widgets/lz_spin_button.h"
#include "details.h"
#include "main_window/summary.h"
#include <libtimeit/db/database.h>
#include <widget_interface.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>

namespace gui
{
using namespace libtimeit;


class details_dialog:
		public Gtk::Dialog,
		public summary_observer,
		public event_observer,
		public time_manager_observer,
		public widget_interface
{
public:
	details_dialog(
			database&       db,
			time_manager&    op_time_keeper,
			notification_manager&       notifier,
			window_manager& window_manager,
			image_cache&         images);
	details_dialog(const details_dialog&) = delete;
	details_dialog(details_dialog&&) = delete;
	details_dialog& operator=(const details_dialog&) = delete;
	details_dialog& operator=(details_dialog&&) = delete;
	~details_dialog() override ;
	void set( optional<task_id> item, time_point<system_clock> start_time, time_point<system_clock> stop_time);


	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position( int& window_x, int& window_y) override { Gtk::Dialog::get_position( window_x, window_y); };

	void on_running_changed() override;
	void on_task_name_updated(const task_id& id);
	void on_task_total_time_updated(const task_id& task_id);

private:

	//SummaryObserver
	void on_selection_changed(
			optional<task_id> id,
			time_point<system_clock> start_time,
			time_point<system_clock> stop_time) override;

	//EventObserver
	void on_task_time_changed(const task_id& id) override;
	void on_task_name_changed(const task& item) override;


	Gtk::Table top_table;
	Gtk::Label task_name;
	Gtk::Image running_image;
	details    detail_list;
	Gtk::Label task_total_time;
	Glib::RefPtr<Gdk::Pixbuf> running_icon;
	Glib::RefPtr<Gdk::Pixbuf> running_idle_icon;
	Glib::RefPtr<Gdk::Pixbuf> blank_icon;
	Gtk::ScrolledWindow scrolled_window;


	time_point<system_clock>  range_start              {system_clock::from_time_t(0)};
	time_point<system_clock>  range_stop               {time_point<system_clock>::max()};

	optional<task_id> presented_task;
	optional<time_id> time_entry_id;

	time_accessor          times;
	extended_task_accessor tasks;
	settings_accessor      settings;
	time_manager&           time_keeper;

	int width;
	int height;
	void size_change( );
};
}

#endif /* DETAILS_DIALOG_H */
