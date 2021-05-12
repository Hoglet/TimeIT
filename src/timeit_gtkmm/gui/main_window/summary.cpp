#include "summary.h"
#include "libtimeit/utils.h"
#include <glibmm/i18n.h>

using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace libtimeit;

namespace gui
{
using namespace std;
using namespace libtimeit;


Summary_observer::~Summary_observer()
{
	unsubscription_allowed = false;
	auto iter = subjects.begin();
	while (iter != subjects.end())
	{
		Summary *subject = *iter;
		subject->detach(this);
		++iter;
	}
}
void Summary_observer::attach(Summary *subject)
{
	subjects.push_back(subject);
}
void Summary_observer::detach(Summary *subject)
{
	if (unsubscription_allowed)
	{
		subjects.remove(subject);
	}
}

Summary::Summary(
		Database &database,
		Notifier& notifier)
		:
		Event_observer(notifier),
		time_accessor(database),
		task_accessor(database)
{
	tree_model = TreeStore::create(columns);
	set_model(tree_model);
	append_column("Name", columns.col_name);
	append_column("Time", columns.col_time);
	set_headers_visible(false);

	ref_tree_selection = get_selection();
	ref_tree_selection->signal_changed().connect(sigc::mem_fun(*this, &Summary::on_selection_changed));

	Gtk::Menu::MenuList &menu_list = menu_popup.items();

	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Show details"), sigc::mem_fun(*this,
																					 &Summary::on_menu_show_details)));

}


bool Summary::on_button_press_event(GdkEventButton *event)
{
	bool return_value = Gtk::TreeView::on_button_press_event(event);
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		menu_popup.popup(event->button, event->time);
		return_value = true; //It has been handled.
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		on_menu_show_details();
		return_value = true;
	}
	return return_value;
}

void Summary::on_menu_show_details()
{
	auto id = selected_id();
	for (auto* observer: observers)
	{
		observer->on_show_details_clicked(id, start_time, stop_time);
	}
}

void Summary::init()
{
}

void Summary::on_selection_changed()
{
	auto id = selected_id();
	for (auto* observer: observers)
	{
		observer->on_selection_changed(id, start_time, stop_time);
	}
}
void Summary::attach(Summary_observer *observer)
{
	observers.push_back(observer);
}

void Summary::detach(Summary_observer *observer)
{
	observer->detach(this);
	observers.remove(observer);
}

Task_id Summary::selected_id()
{
	Task_id ret_val = 0;
	auto tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = tree_selection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row;
		row = *iter;
		ret_val = row[columns.col_id];
	}
	return ret_val;
}
void Summary::set_references(Gtk::Calendar &cal)
{
	calendar = &cal;
	connect_signals();
	on_date_changed();
}
void Summary::connect_signals()
{
	calendar->signal_day_selected().connect(sigc::mem_fun(this, &Summary::on_date_changed));
	calendar->signal_day_selected_double_click().connect(sigc::mem_fun(this, &Summary::on_date_changed));
	calendar->signal_month_changed().connect(sigc::mem_fun(this, &Summary::on_date_changed));
	calendar->signal_next_month().connect(sigc::mem_fun(this, &Summary::on_date_changed));
	calendar->signal_next_year().connect(sigc::mem_fun(this, &Summary::on_date_changed));
	calendar->signal_prev_month().connect(sigc::mem_fun(this, &Summary::on_date_changed));
	calendar->signal_prev_year().connect(sigc::mem_fun(this, &Summary::on_date_changed));
}

void Summary::calculate_time_span()
{
}

void Summary::on_task_updated(Task_id taskID)
{
	if (is_visible())
	{
		auto task = task_accessor.by_ID(taskID);
		Gtk::TreeIter iter = find_row(taskID);
		if (task.has_value() && iter != tree_model->children().end())
		{
			TreeModel::Row row = *iter;
			time_t total_time = time_accessor.total_cumulative_time(taskID, start_time, stop_time);
			assign_values_to_row(row, *task, total_time);
			int64_t parent_id = task->parent_id;
			if (parent_id > 0)
			{
				on_task_updated(parent_id);
			}
		}
		else
		{
			empty();
			populate();
		}
	}
	else
	{
		needs_re_population = true;
	}
}

void Summary::on_task_name_changed(int64_t id)
{
	on_task_updated(id);
}

void Summary::on_task_time_changed(int64_t id)
{
	on_task_updated(id);
}

void Summary::on_complete_update()
{
	if (is_visible())
	{
		empty();
		populate();
	}
	else
	{
		needs_re_population = true;
	}
}

void Summary::on_task_removed(int64_t taskID)
{
	Gtk::TreeIter iter = find_row(taskID);
	if (iter != tree_model->children().end())
	{
		tree_model->erase(iter);
	}
}

void Summary::on_date_changed()
{
	guint year{0};
	guint month{0};
	guint day{0};
	calendar->get_date(year, month, day);
	active_day = libtimeit::to_time(year, month, day); // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
	auto old_start_time = start_time;
	auto old_stop_time  = stop_time;
	calculate_time_span();
	if (
			needs_re_population ||
			old_start_time != start_time ||
			old_stop_time != stop_time )
	{
		if (is_visible())
		{
			empty();
			populate();
		}
		else
		{
			needs_re_population = true;
		}
	}
}
void Summary::empty()
{
	tree_model->clear();
}

bool Summary::on_focus(Gtk::DirectionType direction)
{
	bool return_value = Gtk::TreeView::on_focus(direction);
	if (needs_re_population)
	{
		empty();
		populate();
	}
	return return_value;
}

bool Summary::is_visible()
{
	Container *parent = this->get_parent();
	auto* notebook = dynamic_cast<Gtk::Notebook*>(parent->get_parent());
	if (notebook != nullptr)
	{
		int active_tab = notebook->get_current_page();
		Widget *active = notebook->get_nth_page(active_tab);
		if (active != parent)
		{
			return false;
		}
	}
	return true;
}

TreeModel::Row Summary::add(int64_t id)
{
	auto task = task_accessor.by_ID(id);
	TreeModel::Row row;

	if (task->parent_id > 0)
	{
		Gtk::TreeIter iter = find_row(task->parent_id);
		if (iter != tree_model->children().end())
		{
			row = *iter;
		}
		else
		{
			row = add(task->parent_id);
		}
		row = *(tree_model->append((row).children()));
	}
	else
	{
		TreeModel::iterator iter = tree_model->append();
		row = *iter;
	}
	auto total_time = time_accessor.total_cumulative_time(id, start_time, stop_time);
	assign_values_to_row(row, *task, total_time);
	return row;
}

void Summary::populate()
{
	if (is_visible())
	{
		auto active_tasks = time_accessor.active_tasks(start_time, stop_time);

		for (auto id : active_tasks)
		{
			add(id);
		}
		needs_re_population = false;
	}
	else
	{
		needs_re_population = true;
	}
}

void Summary::assign_values_to_row(TreeModel::Row &row, Task& task, time_t total_time) const
{
	row[columns.col_id]   = task.id;
	row[columns.col_name] = task.name;
	row[columns.col_time] = libtimeit::seconds_2_hhmm(total_time);
}

Gtk::TreeModel::iterator Summary::find_row(Task_id id)
{
	TreeModel::Children children = tree_model->children();
	return sub_search(id, children);
}

Gtk::TreeModel::iterator Summary::sub_search(Task_id id, TreeModel::Children children)
{
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		TreeModel::Row row = *iter;
		if ( ! row.children().empty() )
		{
			auto sub_iter = sub_search(id, row.children());
			if (sub_iter != row.children().end())
			{
				iter = sub_iter;
				break;
			}
		}
		if (row[columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

}
