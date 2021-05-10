#ifndef DETAILS_DIALOG_H_
#define DETAILS_DIALOG_H_

#include <gtkmm.h>
#include "lz_spin_button.h"
#include "details.h"
#include "main_window/summary.h"
#include <libtimeit/db/database.h>
#include <IWidget.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>

namespace gui
{
using namespace libtimeit;


class DetailsDialog:
		public Gtk::Dialog,
		public SummaryObserver,
		public Event_observer,
		public IWidget
{
public:
	static std::shared_ptr<DetailsDialog> create(Database& database, Time_keeper &timeKeeper, Notifier& notifier, GUIFactory  &gui_factory);
	DetailsDialog(const DetailsDialog&) = delete;
	DetailsDialog( DetailsDialog&&) = delete;
	DetailsDialog& operator=(const DetailsDialog&) = delete;
	DetailsDialog& operator=(DetailsDialog&&) = delete;
	~DetailsDialog() override ;
	void set(int64_t ID,time_t startTime,time_t stopTime);


	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };

	void on_runningTasksChanged();
	void on_task_name_updated(int64_t task_id);
	void on_task_total_time_updated(int64_t task_id);

private:
	DetailsDialog(Database& database, Time_keeper &timeKeeper, Notifier& notifier, GUIFactory  &gui_factory);

	//SummaryObserver
	void on_selection_changed(int64_t ID,time_t startTime,time_t stopTime) override;
	void on_show_details_clicked(int64_t taskId, time_t startTime, time_t stopTime) override {};

	//EventObserver
	void on_task_time_changed(int64_t task_ID) override;
	void on_task_name_changed(int64_t task_ID) override;


	Gtk::Table table1;
	Gtk::Label taskName;
	Gtk::Image runningImage;
	Details    detailList;
	Gtk::Label taskTotalTime;
	Glib::RefPtr<Gdk::Pixbuf> runningIcon;
	Glib::RefPtr<Gdk::Pixbuf> runningIdleIcon;
	Glib::RefPtr<Gdk::Pixbuf> blankIcon;
	Gtk::Table table2;
	Gtk::ScrolledWindow scrolledWindow;


	time_t  range_start;
	time_t  range_stop;
	int64_t task_id;
	int64_t time_entry_id;
	Time_accessor          m_timeAccessor;
	Extended_task_accessor m_taskAccessor;
	Settings_accessor      settings_accessor;
	Time_keeper&           m_timeKeeper;
};
}

#endif /* DETAILS_DIALOG_H_ */
