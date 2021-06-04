/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARY_H_
#define SUMMARY_H_

#include <gtkmm.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>

namespace gui
{

using namespace libtimeit;
using namespace std;

class Summary;
class Summary_observer
{

public:
	Summary_observer() = default;
	Summary_observer(const Summary_observer& )            = delete;
	Summary_observer(Summary_observer&& )                 = delete;
	Summary_observer& operator=(const Summary_observer& ) = delete;
	Summary_observer& operator=(Summary_observer&& )      = delete;
	virtual ~Summary_observer();
	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime) = 0;
	virtual void on_show_details_clicked(int64_t /*task_id*/, time_t /*start_time*/, time_t /*stop_time*/) {};
	void attach(Summary *subject);
	void detach(Summary *subject);
private:
	bool unsubscription_allowed{true};
	std::list<Summary*> subjects;
};


class Summary: public Gtk::TreeView, public Event_observer
{
public:
	Summary(Database &database, Notifier& notifier);
	void set_references(Gtk::Calendar &calendar);
	Task_id selected_id();
	time_t get_start_time() const
	{
		return start_time;
	}
	time_t get_stop_time() const
	{
		return stop_time;
	}
	void attach(Summary_observer* /*observer*/);
	void detach(Summary_observer* /*observer*/);
	bool on_focus(Gtk::DirectionType direction) override;

protected:
	void init();
	time_t active_day = 0; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
	time_t start_time = 0; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
	time_t stop_time = 0;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

private:
	Gtk::TreeModel::Row add(int64_t id);
	Gtk::Menu menu_popup;

	bool on_button_press_event(GdkEventButton *event) override;

	//SummaryObserver
	virtual void on_selection_changed();

	//
	void connect_signals();
	void on_date_changed();
	void on_menu_show_details();
	void on_task_updated(Task_id id) override;
	void on_task_removed(Task_id id) override;
	void on_complete_update() override;
	void on_task_name_changed(Task_id id) override;
	void on_task_time_changed(Task_id id) override;
	void populate();
	void empty();
	virtual void calculate_time_span() = 0;
	Gtk::TreeModel::iterator find_row(Task_id id);
	Gtk::TreeModel::iterator sub_search(Task_id id, Gtk::TreeModel::Children children);
	void assign_values_to_row(Gtk::TreeModel::Row &row, Task& task, time_t total_time) const;

	Glib::RefPtr<Gtk::TreeStore> tree_model;
	class Model_columns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		Model_columns()
		{

			add(col_id);
			add(col_name);
			add(col_time);
		}
		;
		Gtk::TreeModelColumn<Task_id>       col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
	};
	Glib::RefPtr<Gtk::TreeSelection> ref_tree_selection;
	Model_columns columns;
	Gtk::Calendar *calendar = nullptr;


	std::list<Summary_observer*> observers;
	Time_accessor time_accessor;
	Task_accessor task_accessor;

	bool is_visible();
	bool needs_re_population = true;
};

}
#endif /* SUMMARY_H_ */
